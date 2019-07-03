//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Buffer for transmiting messages to MQTT
//

#define MESSAGE_BUFFER_SIZE 200
#define MESSAGE_TYPE_PAIR 0
#define MESSAGE_TYPE_TEAM 1
Task sendingTask(100, TASK_FOREVER, &sendingTaskCb, &scheduler, true);


struct __attribute__((__packed__)) Message {
  uint32_t time;
  uint16_t robotId: 12;
  uint8_t teamId: 2;
  uint8_t type: 2;
  uint16_t seen;
} sendingBuffer[MESSAGE_BUFFER_SIZE];
uint16_t sendingBufferStart;
uint16_t sendingBufferEnd;

void sendingAddMessage(struct Message* msg) {
  memcpy(&(sendingBuffer[sendingBufferStart]), msg, sizeof(Message));
  sendingBufferStart = (sendingBufferStart + 1) % MESSAGE_BUFFER_SIZE;
  if (sendingBufferStart == sendingBufferEnd) {
    sendingBufferEnd = (sendingBufferEnd + 1) % MESSAGE_BUFFER_SIZE;
  }
}

bool sendingPeekMessage(struct Message* value) {
  if (sendingBufferStart == sendingBufferEnd) return false;
  memcpy(value, &(sendingBuffer[sendingBufferEnd]), sizeof(Message));
  return true;
}

bool sendingRemoveMessage() {
  if (sendingBufferStart == sendingBufferEnd) return false;
  sendingBufferEnd = (sendingBufferEnd + 1) % MESSAGE_BUFFER_SIZE;
  return true;
}

bool sendingDoSend(Message& msg) {
  String payload;
  payload.reserve(128);
  payload = "{\"cmd\":\"";
  payload += msg.type == MESSAGE_TYPE_PAIR ? "pair" : "team";
  payload += "\",\"time\":";
  payload += msg.time;
  payload += ",\"robotId\":";
  payload += msg.robotId;
  payload += ",\"teamId\":";
  payload += msg.teamId;
  payload += ",\"seen\":";
  payload += msg.seen;
  payload += '}';

  return mqttClient.publish(mqttControlTopic, payload);
}

void sendingTaskCb() {
  if (!mqttClient.connected()) {
    return;
  }

  Message msg;
  if (!sendingPeekMessage(&msg)) return;

  logInfo("Sending message");
  if (sendingDoSend(msg)) {
    sendingRemoveMessage();
  }
}
