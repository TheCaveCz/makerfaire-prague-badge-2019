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
#if LOG_ENABLED || SERIAL_COMMANDS
  Serial.begin(19200);
#endif
  logInfo("Starting");

  dispSetup();
  btnSetup();
  irSendSetup();
  irRecvSetup();
  storageSetup();

  // used for random delays when transmitting presence pulse
  randomSeed(storageMyId);

#if SERIAL_COMMANDS
  logInfo("Serial commands enabled");
  cmd.begin(commands);
#endif

  logInfo("Ready");
}

void loop() {
#if SERIAL_COMMANDS
  cmd.check();
#endif
  btnTick();
  scheduler.execute();
}
