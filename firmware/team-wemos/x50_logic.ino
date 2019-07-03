//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Main team picker logic 
//

Task logicTimeout(100, TASK_FOREVER, &logicTimeoutCb, &scheduler, false);
Task logicStatusTask(60000, TASK_FOREVER, &logicSendStatusCb, &scheduler, true);


typedef enum {
  LS_READY,
  LS_PAIR_INFO,
  LS_TEAM_FULL,
  LS_PAIR_REQUEST,
  LS_TRY_AGAIN,
  LS_ALREADY_TEAMED,
  LS_PAIR_SUCCESS,
} LogicState;

typedef enum {
  LT_UNDEFINED,
  LT_RED,
  LT_GREEN,
  LT_BLUE,
} LogicTeam;


bool logicCanJoinRed;
bool logicCanJoinGreen;
bool logicCanJoinBlue;

uint16_t logicSeenRobotId;
LogicTeam logicSeenRobotTeam;
uint16_t logicSeenRobotCount;

LogicTeam logicJoiningTeam;
uint8_t logicTeamDelta;

LogicState logicState;

void logicSetState(LogicState newState);
bool logicCanJoin(LogicTeam team);

void logicSetup() {
  mqttOnConnect = logicOnMqttConnect;
  mqttOnMessage = logicOnMqttMessage;
  commCallback = logicOnSerialMessage;

  logicTeamDelta = LOGIC_TEAM_DEFAULT_DELTA;

  logicSetState(LS_READY);
}

bool logicAllowedDifference(const uint16_t main, const uint16_t o1, const uint16_t o2) {
  int32_t delta1 = main;
  int32_t delta2 = main;
  delta1 -= o1;
  delta2 -= o2;
  return (delta1 < logicTeamDelta) && (delta2 < logicTeamDelta);
}

bool logicCanJoin(LogicTeam team) {
  switch (team) {
    case LT_RED: return logicCanJoinRed;
    case LT_GREEN: return logicCanJoinGreen;
    case LT_BLUE: return logicCanJoinBlue;
    default: return false;
  }
}

void logicSendStatusCb() {
  if (!mqttClient.connected()) {
    return;
  }

  String payload;
  payload.reserve(128);
  payload = "{\"cmd\":\"status\",\"r\":";
  payload += storageTeams.red;
  payload += ",\"g\":";
  payload += storageTeams.green;
  payload += ",\"b\":";
  payload += storageTeams.blue;
  payload += ",\"delta\":";
  payload += logicTeamDelta;
  payload += ",\"uptime\":";
  payload += millis();
  payload += ",\"bufferEmpty\":";
  payload += sendingBufferEmpty() ? '1' : '0';
  payload += '}';
  mqttClient.publish(mqttControlTopic, payload);
}

void logicOnMqttConnect() {
  logicStatusTask.forceNextIteration();
}

void logicOnMqttMessage(const String& message) {
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(message);
  if (!root.success()) {
    logValue("Json parsing failed: ", message);
    return;
  }

  const char* cmd = root["cmd"];
  if (!cmd) return;
  logValue("Command: ", cmd);

  if (strcmp(cmd, "setValues") == 0) {
    storageTeams.red = root["r"];
    storageTeams.green = root["g"];
    storageTeams.blue = root["b"];
    logicUpdateTeamCount();
    storageSaveScore();
    logicStatusTask.forceNextIteration();

  } else if (strcmp(cmd, "setDelta") == 0) {
    logicTeamDelta = root["delta"];
    logValue("New team delta: ", logicTeamDelta);
    logicUpdateTeamCount();

  } else if (strcmp(cmd, "getStatus") == 0) {
    logicStatusTask.forceNextIteration();
  }
}

void logicParsePairMessage(const char * buf) {
  //P067C2000A
  logicSeenRobotId = parseHexWord(buf, 1);
  logicSeenRobotTeam = (LogicTeam)parseHex(buf[5]);
  logicSeenRobotCount = parseHexWord(buf, 6);
}

void logicSendMqtt(const uint8_t type) {
  Message msg;
  msg.time = millis();
  msg.robotId = logicSeenRobotId;
  msg.teamId = logicSeenRobotTeam;
  msg.seen = logicSeenRobotCount;
  msg.type = type;
  sendingAddMessage(&msg);
}

void logicOnSerialMessage(const char * buf) {
  if (logicState == LS_READY) {
    if (buf[0] == 'B') { // button press
      logicJoiningTeam = (LogicTeam)(buf[1] - '0');
      logicSetState(logicCanJoin(logicJoiningTeam) ? LS_PAIR_REQUEST : LS_TEAM_FULL);

    } else if (buf[0] == 'P') { // pairing
      logicParsePairMessage(buf);
      logicSendMqtt(MESSAGE_TYPE_PAIR);
      logicSetState(LS_PAIR_INFO);
    }



  } else if (logicState == LS_PAIR_REQUEST) {
    if (buf[0] == 'P') { // pairing when team -> already has a team
      logicParsePairMessage(buf);
      logicSendMqtt(MESSAGE_TYPE_PAIR);
      logicSetState(LS_ALREADY_TEAMED);

    } else if (buf[0] == 'T') { // team set success
      logicParsePairMessage(buf);
      switch (logicSeenRobotTeam) {
        case LT_RED: storageTeams.red++; break;
        case LT_GREEN: storageTeams.green++; break;
        case LT_BLUE: storageTeams.blue++; break;
        default: break;
      }
      storageSaveScore();

      logicSendMqtt(MESSAGE_TYPE_TEAM);
      logicSetState(LS_PAIR_SUCCESS);
      logicStatusTask.forceNextIteration();

    } else if (buf[0] == 'Y') { // pairing started
      logicTimeout.disable();

    } else if (buf[0] == 'X') { // pairing timed out in communication
      logicSetState(LS_TRY_AGAIN);
    }


  } else {
    // all other logic states
    if (buf[0] == 'B') { // button press
      logicTimeoutAction();
    }
  }
}

void logicTimeoutAction() {
  logicSetState(LS_READY);
}

void logicTimeoutCb() {
  // using multiple iterations because of display
  if (logicTimeout.isLastIteration()) {
    logicTimeoutAction();
  }
}

void logicSetState(LogicState newState) {
  logicState = newState;
  logValue("Set state: ", logicState);
  switch (logicState) {
    case LS_READY:
      Serial.println("T0");
      logicSeenRobotId = 0;
      logicSeenRobotTeam = LT_UNDEFINED;
      logicSeenRobotCount = 0;
      logicJoiningTeam = LT_UNDEFINED;
      logicTimeout.disable();
      break;

    case LS_PAIR_INFO:
    case LS_TEAM_FULL:
    case LS_TRY_AGAIN:
    case LS_ALREADY_TEAMED:
    case LS_PAIR_SUCCESS:
      logicTimeout.setIterations(100);
      logicTimeout.restart();
      break;

    case LS_PAIR_REQUEST:
      Serial.write('T');
      Serial.write(logicJoiningTeam + '0');
      Serial.println();
      logicTimeout.setIterations(200);
      logicTimeout.restart();
      break;

    default: break;
  }

  logicUpdateTeamCount();
  dispClear();
}

void logicUpdateTeamCount() {
  logicCanJoinRed = logicAllowedDifference(storageTeams.red, storageTeams.green, storageTeams.blue);
  logicCanJoinGreen = logicAllowedDifference(storageTeams.green, storageTeams.red, storageTeams.blue);
  logicCanJoinBlue = logicAllowedDifference(storageTeams.blue, storageTeams.red, storageTeams.green);

  Serial.write('D');
  printHexWord(storageTeams.red);
  printHexWord(storageTeams.green);
  printHexWord(storageTeams.blue);
  Serial.println();

  logicUpdateButtons();
}

void logicUpdateButtons() {
  Serial.write('B');
  if (logicState == LS_READY) {
    Serial.write(logicCanJoinRed ? '2' : '0');
    Serial.write(logicCanJoinGreen ? '2' : '0');
    Serial.write(logicCanJoinBlue ? '2' : '0');
  } else if (logicState == LS_PAIR_REQUEST) {
    Serial.write(logicJoiningTeam == LT_RED ? '1' : '0');
    Serial.write(logicJoiningTeam == LT_GREEN ? '1' : '0');
    Serial.write(logicJoiningTeam == LT_BLUE ? '1' : '0');
  } else if (logicState == LS_PAIR_INFO && logicSeenRobotTeam == LT_UNDEFINED) {
    Serial.print("111");
  } else {
    Serial.print("000");
  }
  Serial.println();
}
