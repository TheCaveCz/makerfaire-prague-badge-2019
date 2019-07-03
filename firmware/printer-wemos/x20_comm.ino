//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Parsing serial commands
//

Task commTask(10, TASK_FOREVER, &commCb, &scheduler, true);

#define COMM_STATE_READY 0
#define COMM_STATE_IGNORE 1
#define COMM_STATE_COMMAND 2


uint8_t commState;
char commBuffer[64];
uint8_t commBufPos;
void (*commCallback)(const char * buf);


void printHex(const uint8_t b) {
  if (b < 16) Serial.print('0');
  Serial.print(b, HEX);
}

void printHexWord(const uint16_t w) {
  printHex(w >> 8);
  printHex(w & 0xff);
}

uint16_t parseHexWord(const char * buf, const uint8_t from) {
  return (parseHex(buf[from], buf[from + 1]) << 8) | parseHex(buf[from + 2], buf[from + 3]);
}

uint8_t parseHex(const uint8_t v1, const uint8_t v2) {
  return (parseHex(v1) << 4) | parseHex(v2);
}

uint8_t parseHex(const uint8_t val) {
  if (val >= '0' && val <= '9') return val - '0';
  if (val >= 'A' && val <= 'F') return val - 'A' + 10;
  if (val >= 'a' && val <= 'f') return val - 'a' + 10;
  return 0;
}


void commSetup() {
  commState = COMM_STATE_READY;
  commCallback = NULL;
  Serial.begin(19200);
  Serial.println();
  Serial.write('!');
  Serial.println(chipId); // prepare for sending (clean old buffer in receiver)
  Serial.println();
  while (Serial.read() >= 0); // drain buffer
}

void commHandleChar(const char r) {
  switch (commState) {
    case COMM_STATE_READY:
      commBufPos = 0;
      // reading first letter, we are only interested in !
      if (r == '!') {
        // incoming command
        commState = COMM_STATE_COMMAND;
      } else {
        commState = COMM_STATE_IGNORE;
      }
      break;

    case COMM_STATE_COMMAND:
      if (r == 10) {
        commBuffer[commBufPos] = 0;
        logValue("Got buffer: ", commBuffer);
        if (commCallback) commCallback(commBuffer);
        commState = COMM_STATE_READY;
      } else {
        commBuffer[commBufPos++] = r;
        if (commBufPos >= sizeof(commBuffer)) {
          logInfo("Incoming buffer overflow");
          commState = COMM_STATE_IGNORE;
        }
      }
      break;

    default:
      // ignoring chars until EOL, then go to ready
      if (r == 10) {
        commState = COMM_STATE_READY;
      }
      break;
  }
}

void commCb() {
  while (Serial.available()) {
    int r = Serial.read();
    if (r < 0 || r == 13) return;
    commHandleChar(r);
  }
}
