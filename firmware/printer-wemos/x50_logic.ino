//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Main dumper logic
//

uint8_t logicPagesCount;
Message logicDump;
bool logicDumpStarted;

void logicSetup() {
  logicDumpStarted = false;

  commCallback = logicOnSerialMessage;
}

void logicResetDump() {
  memset(&logicDump, 0, sizeof(Message));
  logicDumpStarted = false;
}

void logicRecordData(const uint8_t pageId, const uint8_t d1, const uint8_t d2, const uint8_t d3) {
  uint16_t a = pageId;
  a *= 3;

  if (a < MESSAGE_DUMP_SIZE) logicDump.dump[a] = d1;
  a++;
  if (a < MESSAGE_DUMP_SIZE) logicDump.dump[a] = d2;
  a++;
  if (a < MESSAGE_DUMP_SIZE) logicDump.dump[a] = d3;
}

void logicOnSerialMessage(const char * buf) {
  if (buf[0] == 'S') { // dump start
    // S0BE10000101
    // 0123456789ab
    logicDumpStarted = true; // TODO : maybe timeout?

    logicDump.robotId = parseHexWord(buf, 1);
    logicDump.teamId = parseHex(buf[5]);
    logicDump.seen = parseHexWord(buf, 6);
    logicPagesCount = parseHex(buf[10], buf[11]);

    logValue("Dump started #", logicDump.robotId);
    logValue("  pages: ", logicPagesCount);

  } else if (buf[0] == 'P' && logicDumpStarted) { // page received
    // P015E800000
    // 0123456789a

    uint8_t pageNo = parseHex(buf[1], buf[2]);
    uint8_t pageId = parseHex(buf[3], buf[4]);
    uint8_t d1 = parseHex(buf[5], buf[6]);
    uint8_t d2 = parseHex(buf[7], buf[8]);
    uint8_t d3 = parseHex(buf[9], buf[10]);

    logValue("Got page ", pageNo);

    logicRecordData(pageId, d1, d2, d3);


  } else if (buf[0] == 'C' && logicDumpStarted) { // dump complete
    logInfo("Dump completed");
    logicDump.time = millis();
    sendingAddMessage(&logicDump);
    logicResetDump();

  } else if (buf[0] == 'E') { // dump failed
    logicResetDump();
  }
}
