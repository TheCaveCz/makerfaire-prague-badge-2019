//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// OTA
//

uint8_t otaProgress;

void otaSetup() {
  String hn = WIFI_AP_NAME;
  hn += chipId;
  ArduinoOTA.setHostname(hn.c_str());
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.onStart([]() {
    logInfo("OTA starting");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("OTA");
    display.display();
  });
  ArduinoOTA.onEnd([]() {
    logInfo("OTA finished");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("OTA done");
    display.display();
  });
  ArduinoOTA.onProgress([](uint32_t now, uint32_t total) {
    uint8_t p = map(now, 0, total, 0, 10);
    if (p != otaProgress) {
      otaProgress = p;
      logValue("OTA progress ", p);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("OTA: ");
      display.print(p);
      display.display();

    }
  });
  ArduinoOTA.begin();
}
