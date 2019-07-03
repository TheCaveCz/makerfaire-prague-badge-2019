//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Log server
//

ESP8266WebServer server(80);

void serverSetup() {
  server.on("/log", HTTP_GET, []() {
    logBuffer.dumpTo(&server);
  });
  server.begin();
}
