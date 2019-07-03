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

  buttonsSetup();
  powerSetup();
  pixelsSetup();
  ledsSetup();
  irSendSetup();
  irRecvSetup();
  storageSetup();
  logicSetup();

  // used for random delays when transmitting presence pulse
  randomSeed(storageMyId);

#if SERIAL_COMMANDS
  logInfo("Serial commands enabled");
  cmd.begin(commands);
#endif

  // blink eyes at start, useful for programming and detecting resets
  // don't mess with leds anim functions, touch outputs directly
  digitalWrite(PIN_LED1, HIGH);
  digitalWrite(PIN_LED2, HIGH);
  delay(150);
  digitalWrite(PIN_LED1, LOW);
  digitalWrite(PIN_LED2, LOW);
  delay(100);
  digitalWrite(PIN_LED1, HIGH);
  digitalWrite(PIN_LED2, HIGH);
  delay(150);
  digitalWrite(PIN_LED1, LOW);
  digitalWrite(PIN_LED2, LOW);

  // we want to sleep after a while after everything is set up
  logicGoToSleep();
  logInfo("Ready");
}

void loop() {
  // after wakeup from sleep logicCheckSleep function will enable tasks needed
  logicCheckSleep();
#if SERIAL_COMMANDS
  cmd.check();
#endif
  scheduler.execute();
}
