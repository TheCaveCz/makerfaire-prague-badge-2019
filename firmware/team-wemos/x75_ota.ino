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
    disp.clear();
    disp.print("OTA");
  });
  ArduinoOTA.onEnd([]() {
    logInfo("OTA finished");

    disp.setCursor(0, 0);
    disp.print("OTA processing...");
  });
  ArduinoOTA.onProgress([](uint32_t now, uint32_t total) {
    uint8_t p = now / (total / 10);
    if (p != otaProgress) {
      otaProgress = p;

      disp.setCursor(0, 0);
      disp.print("OTA: ");
      disp.print(otaProgress * 10);
      disp.print(" %   ");
    }
  });
  ArduinoOTA.begin();
}
