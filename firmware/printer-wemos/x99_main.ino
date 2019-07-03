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
  logInfo("Starting");
  logValue("chipId: ", chipId);

  commSetup();
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
}
