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

// first stage of pairing logic
// awaits foreign presence command or dump command
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

    irCommandSend(IR_CMD_PAIR_ACK, storageMyTeam, storageMyId, c.fields.id1);
    logicPairPresenceBadgeId = c.fields.id1;

    logicPairTask.enable(); // this resets the run counter - needed for timeout on next callback
    logicPairTask.setCallback(&logicPairAwaitConfirmCb);

  } else if (c.fields.cmd == IR_CMD_DUMP_REQUEST) {
    // We got dump command from badge printer
    logicDumpRespond(c);
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

    // Well, we have a match. This function will handle restarting pairing logic.
    logicPairBadgeConfirmed(c.fields.id1);
  }
}

// called as a final third step - we can store new ID and restart pairing logic
void logicPairBadgeConfirmed(uint16_t id) {
  // blink LEDs differently if we've seen the badge before
  if (storageIdSeen(id)) {
    logValue("Confirmed prev seen badge ", id);
    ledsSetAnim(ledsAnimDuplicate);
  } else {
    logValue("Confirmed new badge ", id);
    ledsSetAnim(ledsAnimAcknowledge);
    storageMarkIdSeen(id);
  }

  // and go to sleep after 3 secs
  logicPairTask.setInterval(3000);
  logicPairTask.setCallback(&logicPairRestart);
}

void logicPairRestart() {
  // prepare IR receiver - discard any old commands
  irRecvCommandsClear();

  // turn off all leds
  ledsReset();

  // reset last seen badge id
  logicPairPresenceBadgeId = 0;

  // watch for presence commands from other badges
  logicPairTask.setCallback(&logicPairAwaitPresenceCb);
  logicPairTask.setInterval(60);
  logicPairTask.restart();
}
