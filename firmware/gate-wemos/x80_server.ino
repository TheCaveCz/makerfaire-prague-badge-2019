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
#if DEBUG_SERVER
  server.on("/entries.txt", HTTP_GET, []() {
    String payload;
    payload.reserve(512);
    payload = "";
    for (uint16_t i = 0; i < STORAGE_MAX; i++) {
      if (storageEntries[i].id) {
        payload += storageEntries[i].id;
        payload += '=';
        payload += storageEntries[i].remaining;
        payload += '\n';
      }
    }
    server.send(200, "text/plain", payload);
  });
  server.onNotFound([]() {
    if (!serverFileRead(server.uri())) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });
#endif
  server.begin();
}

bool serverFileRead(String path) {
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, "application/octet-stream");
    file.close();
    return true;
  }
  return false;
}
