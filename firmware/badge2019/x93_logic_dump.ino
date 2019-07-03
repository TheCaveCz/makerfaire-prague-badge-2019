//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Handles badge dumping - transmitting all IDs of other badges we've seen
//

// Address range of badge IDs is 12 bits -> 4096 possible values. Internally this is stored as 512 bytes (512*8 -> 4096).
// It would take too long to transmit all 512 bytes, so this data is divided into 3 byte pages (171 in total - 512/3).
// Page only gets transmitted if there are some seen badges in it -> this saves space.
// Dump commands have special structure to fit page address and 3 bytes (see IrCommand.Dump struct in x42_ir_cmds)

#define LOGIC_DUMP_PAGE_SIZE 3
#define LOGIC_DUMP_MAX_PAGE (STORAGE_COUNT/LOGIC_DUMP_PAGE_SIZE+1)
#define LOGIC_DUMP_INVALID_PAGE 0xff

uint8_t logicDumpPage; // current page of badge data we are dumping to master


// used for dumping - see x93_logic_dump for more details
uint8_t logicDumpSeenPagesCount() {
  uint8_t result = 0;
  uint8_t inPageResult = 0;
  uint8_t pageCounter = 0;

  for (uint16_t i = 0; i < STORAGE_COUNT; i++) {
    if (storageSeenIds[i]) inPageResult = 1;

    pageCounter++;
    if (pageCounter == LOGIC_DUMP_PAGE_SIZE) {
      result += inPageResult;
      inPageResult = 0;
      pageCounter = 0;
    }
  }

  if (pageCounter) result += inPageResult;

  return result;
}

uint8_t logicDumpFindNonEmptyPage(const uint8_t startPage) {
  if (startPage >= LOGIC_DUMP_MAX_PAGE) return LOGIC_DUMP_INVALID_PAGE;

  for (uint16_t i = startPage * LOGIC_DUMP_PAGE_SIZE; i < STORAGE_COUNT; i++) {
    if (storageSeenIds[i]) {
      uint8_t result = i / LOGIC_DUMP_PAGE_SIZE;
      logValue("Found nonempty page ", result);
      return result;
    }
  }
  logInfo("No more pages");
  return LOGIC_DUMP_INVALID_PAGE;
}

// second stage of pairing logic
// we received dump command so now we are sending all the IDs we've seen
void logicDumpCb() {
  ledsSetBuiltin(true);
  if (logicPairTask.getRunCounter() >= 30) { // timeout in 3 sec
    logInfo("Timeout waiting for dump confirmation");
    logicPairSleepCb();
    return;
  }

  // Send dump data for current page and wait for confirmation for 1 sec, if we have no confirmation, repeat two other times and then fail and go to sleep.
  if ((logicPairTask.getRunCounter() % 10) == 1) {
    logValue("Dumping page ", logicDumpPage);
    uint16_t addr = logicDumpPage * LOGIC_DUMP_PAGE_SIZE;
    irCommandSendDump(logicDumpPage, storageGetSeenByte(addr), storageGetSeenByte(addr + 1), storageGetSeenByte(addr + 2));
    return;
  }

  IrCommand c;
  if (!irCommandReceived(c)) return;

  if (c.fields.cmd == IR_CMD_DUMP_ACK) {
    // our dump was acknowledged
    // id1 must contain our ID
    if (c.fields.id1 != storageMyId) {
      logInfo("This dump ack is not ours");
      return;
    }
    // and id2 must contain our page
    if (c.fields.id2 != logicDumpPage) {
      logInfo("Dump ack for invalid page");
      return;
    }

    // increase page counter
    logicDumpPage = logicDumpFindNonEmptyPage(logicDumpPage + 1);
    // blink with LED
    ledsSetBuiltin(false);
    if (logicDumpPage == LOGIC_DUMP_INVALID_PAGE) {
      // all pages are out and acknowledged -> we are done, go to sleep after dumping
      logInfo("Dumping finished");
      logicPairSleepCb();
    } else {
      // we still have some pages left
      logicPairTask.enable(); // reset run counter for next page
    }
  }
}

// response to IR_CMD_DUMP_REQUEST
void logicDumpRespond(IrCommand& c) {
  if (!logicPairPresenceSent) {
    logInfo("Got dump without presence");
    return;
  }
  // id1 must contain our ID
  if (c.fields.id1 != storageMyId) {
    logInfo("This dump is not ours");
    return;
  }

  // calculate how many pages we need to dump and respond
  uint8_t seenPagesCount = logicDumpSeenPagesCount();
  irCommandSend(IR_CMD_DUMP_INFO, storageMyTeam, storageMyId, seenPagesCount);

  // find first page to dump
  logicDumpPage = logicDumpFindNonEmptyPage(0);

  if (seenPagesCount && logicDumpPage != LOGIC_DUMP_INVALID_PAGE) {
    // enter dump mode
    ledsSetAnim(ledsAnimDump);
    logicPairSendPresenceTask.disable(); // stop sending presence commands

    logicPairTask.setCallback(&logicDumpCb);
    logicPairTask.enable(); // reset run counter

  } else {
    // nothing to dump, go to sleep
    logInfo("Nothing to dump");
    logicPairSleepCb();
  }
}
