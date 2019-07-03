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
  Serial.println(F("+MFGate2019/1.0"));
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

void cmdSetId(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  if (cmd.checkHex(buf, 1, 4)) {
    uint16_t newId = cmd.parseHexWord(buf, 1);
    if (storageSetId(newId)) {
      cmdGetId(buf, len);
      return;
    }
  }
  cmd.printError();
}

void cmdGetDisp(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  Serial.write('+');
  Serial.write('D');
  cmd.printHexWord(dispValues[0]);
  cmd.printHexWord(dispValues[1]);
  cmd.printHexWord(dispValues[2]);
  Serial.println();
}

void cmdSetDisp(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  if (cmd.checkHex(buf, 1, 12)) {
    dispValues[0] = cmd.parseHexWord(buf, 1);
    dispValues[1] = cmd.parseHexWord(buf, 5);
    dispValues[2] = cmd.parseHexWord(buf, 9);
    dispUpdate();
    cmdGetDisp(buf, len);
  } else {
    cmd.printError();
  }
}

void cmdSetBtn(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  if (cmd.checkRange(buf[1], '0', '2') && cmd.checkRange(buf[2], '0', '2') && cmd.checkRange(buf[3], '0', '2')) {
    btnBlink[0] = buf[1] - '0';
    btnBlink[1] = buf[2] - '0';
    btnBlink[2] = buf[3] - '0';
    btnTask.forceNextIteration();
    cmdGetBtn(buf, len);
  } else {
    cmd.printError();
  }
}

void cmdGetBtn(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  Serial.write('+');
  Serial.write('B');
  Serial.write('0' + btnBlink[0]);
  Serial.write('0' + btnBlink[1]);
  Serial.write('0' + btnBlink[2]);
  Serial.println();
}

void cmdSetPairTeam(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  if (cmd.checkHex(buf[1])) {
    uint8_t team = cmd.parseHex(buf[1]);
    if (team < STORAGE_MAX_TEAM) {
      logicPairAssignedTeam = team;
      cmd.printOk();
      return;
    }
  }
  cmd.printError();
}


Command commands[] = {
  {'I', 1, cmdGetId},
  {'I', 5, cmdSetId},
  {'D', 1, cmdGetDisp},
  {'D', 13, cmdSetDisp},
  {'B', 4, cmdSetBtn},
  {'B', 1, cmdGetBtn},
  {'T', 2, cmdSetPairTeam},
  {'?', 1, cmdInfo},
  {'H', 1, cmdHelp},
  {0, 0, NULL}
};

#endif
