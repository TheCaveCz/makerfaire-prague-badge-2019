//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Handling the IR communication.
//

Task logicPairSendPresenceTask(1000, 4, &logicPairSendPresenceCb, &scheduler, false);
Task logicPairTask(60, TASK_FOREVER, &logicPairAwaitPresenceCb, &scheduler, false);

uint16_t logicPairPresenceBadgeId;
uint8_t logicPairPresenceSent;

// send three presence commands with random delay and then go to sleep
// this task effectively serves as timeout if no other badges are nearby
// it will be disabled once we get other device to talk to us
void logicPairSendPresenceCb() {
  logValue("Send presence task: ", logicPairSendPresenceTask.getRunCounter());
  if (logicPairSendPresenceTask.isFirstIteration()) {
    ledsSetAnim(ledsAnimPresence);
  }
  if (logicPairSendPresenceTask.isLastIteration()) {
    // if we timed out we must disable the other pairing task (so it will not interfere on next wake-up)
    logicPairSleepCb();
    return;
  }

  irCommandSend(IR_CMD_PRESENCE, storageMyTeam, storageMyId, storageSeenCount);
  logicPairPresenceSent = 1;
  logicPairSendPresenceTask.setInterval(random(1600, 2000));
}

// callback for logicPairTask, simply prepares badge for sleep mode
void logicPairSleepCb() {
  logInfo("Pair sleep task triggered");
  logicPairTask.disable();
  logicPairSendPresenceTask.disable();
  pixelsShowId(0);

  logicGoToSleep();
}

// called as a final third step - we can store new ID and go to sleep afterwards
void logicPairBadgeConfirmed(uint16_t id) {
  if (id == 0) {
    // ID 0 is only for new team ID
    logValue("Confirmed new team ", storageMyTeam);
    ledsSetAnim(ledsAnimAcknowledge);
  } else if (storageIdSeen(id)) {
    // blink eyes differently if we've seen the badge before
    logValue("Confirmed prev seen badge ", id);
    ledsSetAnim(ledsAnimDuplicate);
  } else {
    logValue("Confirmed new badge ", id);
    ledsSetAnim(ledsAnimAcknowledge);
    if (id != STORAGE_WILDCARD_ID) {
      storageMarkIdSeen(id);
    }
  }
  // show the other ID
  pixelsShowId(id ? : logicMyTeamId());

  // light LED on the back to indicate pairing is complete
  ledsSetBuiltin(true);

  // and go to sleep after 3 secs
  logicPairTask.setInterval(3000);
  logicPairTask.setCallback(&logicPairSleepCb);
}

// second stage of pairing logic
// we received presence command from other badge and sent our acknowledge
// now we wait for confirmation
void logicPairAwaitConfirmCb() {
  if (logicPairTask.getRunCounter() > 50) { // timeout in 3 sec
    logInfo("Timeout waiting for confirmation");
    logicPairSleepCb();
    return;
  }

  IrCommand c;
  if (!irCommandReceived(c)) return;

  if (c.fields.cmd == IR_CMD_PAIR_CONFIRM || c.fields.cmd == IR_CMD_CONFIRM_OLD) {
    bool isOld = c.fields.cmd == IR_CMD_CONFIRM_OLD;
    uint16_t myId = storageMyId;

    if (isOld) {
      myId &= 0x1ff;
    }
    // id2 contains our ID
    if (c.fields.id2 != myId) {
      logInfo("This confirm is not ours");
      return;
    }
    // id1 contains valid ID of the other badge
    if (!storageIdValid(c.fields.id1, isOld)) {
      logInfo("Invalid badge id");
      return;
    }

    // check that this is actually the badgeID that we got presence from
    if (c.fields.id1 != logicPairPresenceBadgeId) {
      logValue("Unexpected badge ID: ", c.fields.id1);
      return;
    }

    // Well, we have a match. This function will handle going to sleep after some time.
    logicPairBadgeConfirmed(c.fields.id1);
  }
}


// first stage of pairing logic
// awaits either foreign presence command or response to our presence command
void logicPairAwaitPresenceCb() {
  IrCommand c;
  if (!irCommandReceived(c)) return;

  if (c.fields.cmd == IR_CMD_PRESENCE || c.fields.cmd == IR_CMD_PRESENCE_OLD) {
    bool isOld = c.fields.cmd == IR_CMD_PRESENCE_OLD;
    // we got presence command from other badge
    // validate and respond with acknowledge
    // then wait for IR_CMD_PAIR_CONFIRM command

    if (isOld && (c.fields.id1 != (~c.fields.id2 & 0x1ff))) {
      logInfo("Invalid presence ID");
      return;
    }
    if (c.fields.id1 == storageMyId) {
      logInfo("Got own id on presence, ignoring");
      return;
    }

    logicPairSendPresenceTask.disable(); // stop sending our presence commands
    if (isOld) {
      irCommandSendOld(IR_CMD_LEGACY_ACKNOWLEDGE, storageMyId & 0x1ff, c.fields.id1);
    } else {
      irCommandSend(IR_CMD_PAIR_ACK, storageMyTeam, storageMyId, c.fields.id1);
    }
    logicPairPresenceBadgeId = c.fields.id1;

    logicPairTask.enable(); // this resets the run counter - needed for timeout on next callback
    logicPairTask.setCallback(&logicPairAwaitConfirmCb);

  } else if (c.fields.cmd == IR_CMD_PAIR_ACK) {
    // we got acknowledge command from other badge (which received our presence command)
    // send confirm command and mark the other badge as seen

    // id2 must contain our ID
    if (c.fields.id2 != storageMyId) {
      logInfo("This ack is not ours");
      return;
    }
    // id1 must contain valid badge ID
    if (!storageIdValid(c.fields.id1)) {
      logInfo("Invalid badge id");
      return;
    }
    if (!logicPairPresenceSent) {
      logInfo("Got ack without presence");
      return;
    }

    logicPairSendPresenceTask.disable(); // stop sending presence commands
    irCommandSend(IR_CMD_PAIR_CONFIRM, storageMyTeam, storageMyId, c.fields.id1);
    logicPairBadgeConfirmed(c.fields.id1); // this will handle going to sleep after some time

  } else if (c.fields.cmd == IR_CMD_TEAM_SET) {
    // we got set team command from team picker
    // check if we don't have team and set it

    // id2 must contain our ID
    if (c.fields.id2 != storageMyId) {
      logInfo("This set is not ours");
      return;
    }

    if (!logicPairPresenceSent) {
      logInfo("Got set without presence");
      return;
    }

    if (storageMyTeam != STORAGE_TEAM_UNDECIDED) {
      logInfo("I'm already on a team");
      return;
    }

    // check that team ID is valid
    if (c.fields.id1 >= STORAGE_MAX_TEAM || c.fields.id1 == STORAGE_TEAM_UNDECIDED) {
      logValue("Invalid team id ", c.fields.id1);
      return;
    }

    logicPairSendPresenceTask.disable(); // stop sending presence commands
    storageSetTeam(c.fields.id1); // store our new team ID
    irCommandSend(IR_CMD_TEAM_CONFIRM, storageMyTeam, storageMyId, c.fields.id1);
    logicPairBadgeConfirmed(0); // this will handle going to sleep after some time

  } else if (c.fields.cmd == IR_CMD_DUMP_REQUEST) {
    // We got dump command from badge printer
    logicDumpRespond(c);
  }
}

void logicStartPair() {
  // blink the eyes
  ledsSetAnim(ledsAnimWakeupShow);

  // turn off all pixels
  pixelsShowId(0);

  // prepare IR receiver - discard any old commands
  irRecvCommandsClear();
  logicPairPresenceSent = 0;

  // reset last seen badge id
  logicPairPresenceBadgeId = 0;

  // start sending presence commands and watch for timing out when nothing gets received
  logicPairSendPresenceTask.restartDelayed(random(100, 500));

  // watch for presence commands from other badges
  // 100ms interval is enough because 32 bits of IR command takes approx 60-80ms to transmit
  logicPairTask.setCallback(&logicPairAwaitPresenceCb);
  logicPairTask.setInterval(60);
  logicPairTask.restart();
}
