//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// WiFi connection
//

WiFiEventHandler wifiIpHandler;

void wifiSetup() {
  if (WiFi.getAutoConnect()) WiFi.setAutoConnect(false);
  if (WiFi.getMode() != WIFI_STA) WiFi.mode(WIFI_STA);

  logInfo("Connecting...");

  wifiIpHandler = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP & evt) {
    // this executes when module reconnects and gets IP from DHCP
    // can be called multiple times
    logValue("Got IP: ", evt.ip);
  });

  if (!WiFi.isConnected()) { // calling WiFi.begin when already connected does all kind of weird stuff
    WiFi.begin(WIFI_NAME, WIFI_PASS);
  }
}
