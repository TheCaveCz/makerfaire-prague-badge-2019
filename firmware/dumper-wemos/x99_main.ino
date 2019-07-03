//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Setup and loop
//

OneButton oneButton(D7, true);

void buttonClick() {
  if (!logicDumpStarted) {
    Serial.println("D");
  }
}

void setup() {
  logInfo("Starting");
  logValue("chipId: ", chipId);

  oneButton.attachClick(buttonClick);

  commSetup();
  displaySetup();
  mqttSetup();
  logicSetup();
  wifiSetup();
  otaSetup();
  serverSetup();

  logInfo("Setup complete");
}

void loop() {
  ArduinoOTA.handle();
  scheduler.execute();
  server.handleClient();
  oneButton.tick();
}
