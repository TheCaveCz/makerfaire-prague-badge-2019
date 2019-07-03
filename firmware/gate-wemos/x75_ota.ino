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
    ringLeds.ClearTo(RgbColor(128, 64, 0));
    ringLeds.Show();
    logInfo("OTA starting");

  });
  ArduinoOTA.onEnd([]() {
    logInfo("OTA finished");

  });
  ArduinoOTA.onProgress([](uint32_t now, uint32_t total) {
    uint8_t p = map(now, 0, total, 0, RING_COUNT - 1);
    if (p != otaProgress) {
      otaProgress = p;
      RgbColor c1 = RgbColor(128, 64, 0);
      RgbColor c2 = RgbColor(0, 64, 128);
      for (uint8_t i = 0; i < RING_COUNT; i++) {
        ringLeds.SetPixelColor(i, i > p ? c1 : c2);
      }
      ringLeds.Show();
    }
  });
  ArduinoOTA.begin();
}
