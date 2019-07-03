//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Buffer for transmitting messages
//

#define MESSAGE_DUMP_SIZE 512
#define MESSAGE_DUMP_COUNT 4096

#define MESSAGE_BUFFER_SIZE 10
Task sendingTask(100, TASK_FOREVER, &sendingTaskCb, &scheduler, true);


struct __attribute__((__packed__)) Message {
  uint32_t time;
  uint16_t robotId: 12;
  uint8_t teamId: 2;
  uint8_t type: 2;
  uint16_t seen;
  uint8_t dump[MESSAGE_DUMP_SIZE];
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
  payload.reserve(512);
  payload = "{\"cmd\":\"dump\",\"time\":";
  payload += msg.time;
  payload += ",\"robotId\":";
  payload += msg.robotId;
  payload += ",\"teamId\":";
  payload += msg.teamId;
  payload += ",\"seen\":";
  payload += msg.seen;
  payload += ",\"dump\":[";

  bool comma = false;
  for (uint16_t i = 0; i < MESSAGE_DUMP_COUNT; i++) {
    if (msg.dump[i >> 3] & (1 << (i & 0b111))) {
      if (comma) {
        payload += ',';
      } else {
        comma = true;
      }
      payload += i;
    }
  }

  payload += ']';
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
