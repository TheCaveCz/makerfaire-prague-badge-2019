//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Handling the IR communication.
//

Task logicPairTask(60, TASK_FOREVER, &logicPairAwaitPresenceCb, &scheduler, true);

uint16_t logicPairPresenceBadgeId;
uint16_t logicPairOtherSeenCount;
uint8_t logicPairAssignedTeam;

// first stage of pairing logic
// awaits either foreign presence command or response to our presence command
void logicPairAwaitPresenceCb() {
  IrCommand c;
  if (!irCommandReceived(c)) return;

  if (c.fields.cmd == IR_CMD_PRESENCE) {
    // we got presence command from other badge
    // validate and respond with acknowledge
    // then wait for IR_CMD_PAIR_CONFIRM command

    if (c.fields.id1 == storageMyId) {
      logInfo("Got own id on presence, ignoring");
      return;
    }

    logValue("Other badge seen count: ", c.fields.id2);

    if (logicPairAssignedTeam == STORAGE_TEAM_UNDECIDED || c.fields.teamId != STORAGE_TEAM_UNDECIDED) {
      irCommandSend(IR_CMD_PAIR_ACK, STORAGE_TEAM_UNDECIDED, storageMyId, c.fields.id1);
    } else {
      irCommandSend(IR_CMD_TEAM_SET, STORAGE_TEAM_UNDECIDED, logicPairAssignedTeam, c.fields.id1);
#if SERIAL_COMMANDS
      Serial.write('!');
      Serial.write('Y');
      Serial.print(logicPairAssignedTeam);
      Serial.println();
#endif
    }
    logicPairPresenceBadgeId = c.fields.id1;
    logicPairOtherSeenCount = c.fields.id2;

    logicPairTask.enable(); // this resets the run counter - needed for timeout on next callback
    logicPairTask.setCallback(&logicPairAwaitConfirmCb);

  }
}

// second stage of pairing logic
// we received presence command from other badge and sent our acknowledge
// now we wait for confirmation
void logicPairAwaitConfirmCb() {
  if (logicPairTask.getRunCounter() > 50) { // timeout in 3 sec
    logInfo("Timeout waiting for confirmation");
    logicPairRestart();
    return;
  }

  IrCommand c;
  if (!irCommandReceived(c)) return;

  if (c.fields.cmd == IR_CMD_PAIR_CONFIRM) {
    // id2 contains our ID
    if (c.fields.id2 != storageMyId) {
      logInfo("This confirm is not ours");
      return;
    }

    // id1 contains valid ID of the other badge
    if (!storageIdValid(c.fields.id1)) {
      logInfo("Invalid badge id");
      return;
    }

    // check that this is actually the badgeID that we got presence from
    if (c.fields.id1 != logicPairPresenceBadgeId) {
      logValue("Unexpected badge ID: ", c.fields.id1);
      return;
    }

    // Well, we have a match. This function will handle going to sleep after some time.
    logValue("Confirmed new badge ", c.fields.id1);
    logValue("Team id ", c.fields.teamId);

#if SERIAL_COMMANDS
    Serial.write('!');
    Serial.write('P');
    cmd.printHexWord(c.fields.id1);
    Serial.print(c.fields.teamId);
    cmd.printHexWord(logicPairOtherSeenCount);
    Serial.println();
#endif

    // must be also cleared here to prevent failed notifications from sending in logicPairRestart
    logicPairAssignedTeam = STORAGE_TEAM_UNDECIDED;

    // and go to sleep after 1 secs
    logicPairTask.setInterval(1000);
    logicPairTask.setCallback(&logicPairRestart);

  } else if (c.fields.cmd == IR_CMD_TEAM_CONFIRM) {
    if (c.fields.id2 != c.fields.teamId) {
      logInfo("This team pick is bogus");
      return;
    }

    // id1 contains valid ID of the other badge
    if (!storageIdValid(c.fields.id1)) {
      logInfo("Invalid badge id");
      return;
    }

    // check that this is actually the badgeID that we got presence from
    if (c.fields.id1 != logicPairPresenceBadgeId) {
      logValue("Unexpected badge ID: ", c.fields.id1);
      return;
    }

    // Well, we have a match. This function will handle going to sleep after some time.
    logValue("Confirmed team set for badge ", c.fields.id1);
    logValue("Team id ", c.fields.teamId);

#if SERIAL_COMMANDS
    Serial.write('!');
    Serial.write('T');
    cmd.printHexWord(c.fields.id1);
    Serial.print(c.fields.teamId);
    cmd.printHexWord(logicPairOtherSeenCount);
    Serial.println();
#endif

    // must be also cleared here to prevent failed notifications from sending in logicPairRestart
    logicPairAssignedTeam = STORAGE_TEAM_UNDECIDED;

    // and go to sleep after 1 secs
    logicPairTask.setInterval(1000);
    logicPairTask.setCallback(&logicPairRestart);
  }
}

void logicPairRestart() {
  logInfo("Restarting pairing logic");

  // prepare IR receiver - discard any old commands
  irRecvCommandsClear();

  // reset last seen badge id
  logicPairPresenceBadgeId = 0;
  logicPairOtherSeenCount = 0;

#if SERIAL_COMMANDS
  // send notification if we reset without actually setting team!
  if (logicPairAssignedTeam != STORAGE_TEAM_UNDECIDED) {
    Serial.println(F("!X"));
  }
#endif
  logicPairAssignedTeam = STORAGE_TEAM_UNDECIDED;

  // watch for presence commands from other badges
  logicPairTask.setCallback(&logicPairAwaitPresenceCb);
  logicPairTask.setInterval(60);
  logicPairTask.restart();
}
