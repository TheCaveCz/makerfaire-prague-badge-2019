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
  Serial.println(F("+MFDumper2019/1.0"));
}

void cmdHelp(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  cmd.printCommands();
}


void cmdStartDump(__attribute__((unused)) const char * buf, __attribute__((unused)) const uint8_t len) {
  if (logicDumpInProgress) {
    cmd.printError();
    return;
  }
  logicSendWildcardDumpRequest();
  cmd.printOk();
}


Command commands[] = {
  {'D', 1, cmdStartDump},
  {'?', 1, cmdInfo},
  {'H', 1, cmdHelp},
  {0, 0, NULL}
};

#endif
