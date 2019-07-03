//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Dumping logic
//

Task logicDumpTask(50, TASK_FOREVER, &logicAwaitPresenceCb, &scheduler, true);

#define LOGIC_WILDCARD_ID 4095

uint16_t logicDumpId;
uint16_t logicDumpSeen;
uint8_t logicDumpCurrentPage;
uint8_t logicDumpPageCount;
uint8_t logicDumpPageMax;
uint8_t logicDumpTeamId;
bool logicDumpInProgress;

void logicDumpSetup() {
  logicResetDump();
}

void logicResetDump() {
  logInfo("Reseting dumper state");
  logicDumpInProgress = false;

#if USE_PRINTER
  dispClear();
#endif
  // state is reset when recieving presence command

  logicDumpTask.setCallback(&logicAwaitPresenceCb);
  logicDumpTask.setInterval(50);
  logicDumpTask.enable();
}

void logicSendError() {
#if SERIAL_COMMANDS
  Serial.println(F("!E"));
#endif
}

void logicDumpCompleted() {
  logInfo("Dump completed");

  uint16_t seen = storageCalcSeenCount();
  if (seen != logicDumpSeen) {
    logValue("Seen and counted seen mismatch: ", seen);
    logicDumpSeen = seen;
  }

#if SERIAL_COMMANDS
  Serial.write('!');
  Serial.write('C');
  printHexWord(seen);
  Serial.println();
#endif

#if USE_PRINTER
  dispShowResult(logicDumpId, seen);

  printerPrintReport(logicDumpId, seen, logicDumpTeamId);
#endif
  logicDumpTask.setCallback(&logicResetDump);
  logicDumpTask.delay(5000);
}


void logicAwaitPresenceCb() {
  IrCommand c;
  if (!irCommandReceived(c)) return;

  if (c.fields.cmd == IR_CMD_PRESENCE) {
    logicSendDumpRequest(c.fields.id1, c.fields.teamId, c.fields.id2);
  }
}

void logicSendWildcardDumpRequest() {
  logicSendDumpRequest(LOGIC_WILDCARD_ID, 0, 0);
}

void logicSendDumpRequest(const uint16_t dumpId, const uint8_t teamId, const uint16_t seen) {
  logicDumpId = dumpId;
  logicDumpSeen = seen;
  logicDumpTeamId = teamId;
  logicDumpCurrentPage = 0;
  logicDumpPageCount = 0;
  logicDumpInProgress = true;
  storageResetDump();

  logValue("Requested dump for id ", logicDumpId);
  irCommandSend(IR_CMD_DUMP_REQUEST, 0, logicDumpId, 0);
#if USE_PRINTER
  dispShowWelcome(logicDumpId);
#endif
  logicDumpTask.setCallback(&logicAwaitDumpInfoCb);
  logicDumpTask.enable(); // restart run counter
}

void logicAwaitDumpInfoCb() {
  if (logicDumpTask.getRunCounter() >= 60) { // timeout in 3 sec
    logInfo("Timeout waiting for dump info");
#if USE_PRINTER
    dispShowError();
#endif
    logicSendError();
    logicDumpTask.setCallback(&logicResetDump);
    logicDumpTask.delay(3000);
    return;
  }

  IrCommand c;
  if (!irCommandReceived(c)) return;

  if (c.fields.cmd == IR_CMD_DUMP_INFO) {
    if (logicDumpId == LOGIC_WILDCARD_ID) {
      logicDumpId = c.fields.id1;
      logicDumpTeamId = c.fields.teamId;
      logicDumpSeen = 0;
    }

    if (c.fields.id1 != logicDumpId) {
      logInfo("Invalid ID");
      return;
    }
    logicDumpPageMax = c.fields.id2;
    logValue("Expecting pages: ", logicDumpPageMax);

#if SERIAL_COMMANDS
    Serial.write('!');
    Serial.write('S');
    printHexWord(logicDumpId);
    Serial.print(logicDumpTeamId);
    printHexWord(logicDumpSeen);
    printHex(logicDumpPageMax);
    Serial.println();
#endif

    if (logicDumpPageMax == 0) {
      // edge case - empty badge
      logicDumpCompleted();
    } else {
      logicDumpTask.setCallback(&logicDumpReceivePagesCb);
      logicDumpTask.enable(); // restart run counter
    }
  }
}

void logicDumpReceivePagesCb() {
  if (logicDumpTask.getRunCounter() >= 60) { // timeout in 3 sec
    logInfo("Timeout waiting for dump page");
#if USE_PRINTER
    dispShowError();
#endif
    logicSendError();
    logicDumpTask.setCallback(&logicResetDump);
    logicDumpTask.delay(3000);
    return;
  }

  if ((logicDumpTask.getRunCounter() % 20) == 0 && logicDumpCurrentPage) {
    // sending repeated acks for last seen page
    logValue("Sending ACK for page ", logicDumpCurrentPage);
    irCommandSend(IR_CMD_DUMP_ACK, 0, logicDumpId, logicDumpCurrentPage);
  }

  IrCommand c;
  if (!irCommandReceived(c)) return;

  if ((c.fields.cmd & IR_CMD_FLAG) != IR_CMD_FLAG) {
    // count pages so we know when we are done
    uint16_t page = c.dump.pageId;
    if (page <= logicDumpCurrentPage) {
      logInfo("Expected page mismatch");
      return;
    }

    storageSaveSeenByte(page * 3, c.dump.data[0]);
    storageSaveSeenByte(page * 3 + 1, c.dump.data[1]);
    storageSaveSeenByte(page * 3 + 2, c.dump.data[2]);

    logValue("Dumped page ", page);
    logHeader();
    logRaw("  data ");
    logNum(c.dump.data[0], HEX);
    logRaw(',');
    logNum(c.dump.data[1], HEX);
    logRaw(',');
    logNum(c.dump.data[2], HEX);
    logLine();

    logicDumpPageCount++;
    logicDumpCurrentPage = page;

    irCommandSend(IR_CMD_DUMP_ACK, 0, logicDumpId, logicDumpCurrentPage);
#if SERIAL_COMMANDS
    Serial.write('!');
    Serial.write('P');
    printHex(logicDumpPageCount);
    printHex(page);
    printHex(c.dump.data[0]);
    printHex(c.dump.data[1]);
    printHex(c.dump.data[2]);
    Serial.println();
#endif
    if (logicDumpPageCount < logicDumpPageMax) {
      logValue("Pages count: ", logicDumpPageCount);
#if USE_PRINTER
      dispShowProgress(logicDumpPageCount, logicDumpPageMax);
#endif
      logicDumpTask.enable(); // restart run counter
    } else {
      logicDumpCompleted();
    }
  }
}
