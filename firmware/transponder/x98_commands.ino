//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Serial commands implementation
//

#if SERIAL_COMMANDS

void cmdInfo(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  Serial.println(F("+MFTransponder2019/1.0"));
}

void cmdHelp(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  cmd.printCommands();
}

void cmdGetId(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  Serial.write('+');
  Serial.write('I');
  cmd.printHexWord(storageMyId);
  Serial.println();
}

void cmdGetSeenCount(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  Serial.write('+');
  Serial.write('C');
  cmd.printHexWord(storageSeenCount);
  Serial.println();
}

void cmdGetTeam(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  Serial.write('+');
  Serial.write('T');
  Serial.write(storageMyTeam + '0');
  Serial.println();
}

void cmdDumpStorage(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  Serial.write('+');
  Serial.write('D');
  for (uint16_t i = 0; i < STORAGE_COUNT; i++) {
    cmd.printHex(storageSeenIds[i]);
  }
  Serial.println();
}


void cmdDumpIds(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  for (uint16_t id = 0; id < STORAGE_MAX_ID; id++) {
    if (!storageIdSeen(id)) continue;
    Serial.write('>');
    cmd.printHexWord(id);
    Serial.println();
  }
  Serial.write('+');
  Serial.write('E');
  Serial.println();
}

void cmdSetTeam(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  uint8_t newTeam = buf[1] - '0';
  if (storageSetTeam(newTeam)) {
    cmdGetTeam(buf, len);
  } else {
    cmd.printError();
  }
}

void cmdSetId(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  if (cmd.checkHex(buf,1,4)) {
    uint16_t newId = cmd.parseHexWord(buf,1);
    if (storageSetId(newId)) {
      cmdGetId(buf, len);
      return;
    }
  }
  cmd.printError();
}

void cmdFormat(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  if (buf[1] == '!') {
    storageFormat();
    Serial.println(F("+F"));
  } else {
    cmd.printError();
  }
}

Command commands[] = {
  {'I', 1, cmdGetId},
  {'I', 5, cmdSetId},
  {'T', 1, cmdGetTeam},
  {'T', 2, cmdSetTeam},
  {'C', 1, cmdGetSeenCount},
  {'D', 1, cmdDumpStorage},
  {'E', 1, cmdDumpIds},
  {'F', 2, cmdFormat},
  {'?', 1, cmdInfo},
  {'H', 1, cmdHelp},
  {0, 0, NULL}
};

#endif
