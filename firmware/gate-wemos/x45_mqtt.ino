//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// MQTT connection
//

WiFiClient mqttNetClient;
MQTTClient mqttClient(512);

Task mqttTask(100, TASK_FOREVER, &mqttConnectCb, &scheduler, true);
char mqttControlTopic[64];
void (*mqttOnConnect)();
void (*mqttOnMessage)(const String& message);


void mqttSetup() {
  mqttOnConnect = NULL;
  mqttOnMessage = NULL;

  mqttNetClient.setTimeout(20000);
  mqttClient.begin(MQTT_HOST, MQTT_PORT, mqttNetClient);

  snprintf(mqttControlTopic, sizeof(mqttControlTopic), MQTT_TOPIC, chipId.c_str());
  logValue("MQTT topic: ", mqttControlTopic);
}

void mqttMessageCb(String& topic, String& payload) {
  //logValue("MQTT message in ", topic);
  if (topic == mqttControlTopic) {
    logValue("Message: ", payload);
    if (mqttOnMessage) mqttOnMessage(payload);
  }
}

void mqttConnect(const long interval) {
  logValue("MQTT reconnect in ", interval);

  mqttTask.setCallback(&mqttConnectCb);
  mqttTask.restartDelayed(interval);
}

void mqttConnectCb() {
  if (!WiFi.isConnected()) {
    return;
  }

  logInfo("Connecting MQTT");
  if (mqttClient.connected()) {
    mqttClient.disconnect();
  }

  char clientId[64];
  snprintf(clientId, sizeof(clientId), MQTT_CLIENT_ID, chipId.c_str());
  if (!mqttClient.connect(clientId, MQTT_USER, MQTT_PASS)) {
    logValue("MQTT connect failed with error: ", mqttClient.lastError());
    mqttConnect(MQTT_RECONNECT_INTERVAL);
    return;
  }

  logInfo("MQTT connected");
  mqttTask.setCallback(mqttRunCb);

  mqttClient.onMessage(&mqttMessageCb);
  mqttClient.subscribe(mqttControlTopic);
  if (mqttOnConnect) mqttOnConnect();
}

void mqttRunCb() {
  if (!mqttClient.loop()) {
    logValue("MQTT client last error: ", mqttClient.lastError());
    mqttConnect(MQTT_RECONNECT_INTERVAL);
  }
}
