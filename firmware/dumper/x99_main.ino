//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Setup and loop
//

void setup() {
#if SERIAL_COMMANDS
  Serial.begin(19200);
#endif

  logInfo("Starting");

#if USE_PRINTER
  dispSetup();
  printerSetup();
#endif
  irSendSetup();
  irRecvSetup();
  logicDumpSetup();

  logInfo("Ready");
#if USE_PRINTER
  dispClear();
#endif

#if SERIAL_COMMANDS
  logInfo("Serial commands enabled");
  cmd.begin(commands);
#endif

}

void loop() {
#if SERIAL_COMMANDS
  cmd.check();
#endif
  scheduler.execute();
}
