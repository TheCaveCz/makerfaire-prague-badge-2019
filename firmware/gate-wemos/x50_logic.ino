//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Main gate logic
//

Task logicStatusTask(60000, TASK_FOREVER, &logicSendStatusCb, &scheduler, true);
Task logicRefreshTask(2500, TASK_FOREVER, &logicUpdateTeamCount, &scheduler, true);

typedef enum {
  LT_UNDEFINED,
  LT_RED,
  LT_GREEN,
  LT_BLUE,
} LogicTeam;

uint16_t logicSeenRobotId;
LogicTeam logicSeenRobotTeam;
uint16_t logicSeenRobotCount;
uint16_t logicGatePairTimeout;

void logicSetup() {
  mqttOnConnect = logicOnMqttConnect;
  mqttOnMessage = logicOnMqttMessage;
  commCallback = logicOnSerialMessage;

  logicGatePairTimeout = GATE_PAIR_DEFAULT_TIMEOUT;

  logicUpdateTeamCount();
  logicUpdateRingAnim();
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
  payload += ",\"timeout\":";
  payload += logicGatePairTimeout;
  payload += ",\"uptime\":";
  payload += millis();
  payload += ",\"onTimeout\":";
  payload += storageCountEntries();
  payload += ",\"inputVoltage\":";
  payload += map(analogRead(A0), 0, 1024, 0, 1320) / 100.0;
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

  if (strcmp(cmd, "addEntry") == 0) {
    uint16_t time = root["time"];
    if (time < 4096) {
      storageAddEntry(root["id"], time ? : logicGatePairTimeout);
    }

  } else if (strcmp(cmd, "removeEntry") == 0) {
    storageRemoveEntry(root["id"]);

  } else if (strcmp(cmd, "setValues") == 0) {
    storageTeams.red = root["r"];
    storageTeams.green = root["g"];
    storageTeams.blue = root["b"];
    logicUpdateTeamCount();
    logicUpdateRingAnim();
    storageSaveScore();

  } else if (strcmp(cmd, "setTimeout") == 0) {
    uint16_t newTimeout = root["timeout"];
    if (newTimeout >= 10 && newTimeout < 4096) {
      logicGatePairTimeout = newTimeout;
      logValue("New timeout set: ", newTimeout);
    } else {
      logValue("Invalid timeout value: ", newTimeout);
    }

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

bool logicAddScore() {
  switch (logicSeenRobotTeam) {
    case LT_RED: storageTeams.red++; break;
    case LT_GREEN: storageTeams.green++; break;
    case LT_BLUE: storageTeams.blue++; break;
    default: return false;
  }
  return true;
}

RgbColor logicGetTeamColor() {
  RgbColor c;
  switch (logicSeenRobotTeam) {
    case LT_RED: c = ringRed; break;
    case LT_GREEN: c = ringGreen; break;
    case LT_BLUE: c = ringBlue; break;
    default: c = ringWhite; break;
  }
  c.Lighten(64);
  return c;
}

void logicUpdateRingAnim() {
  if (storageTeams.red > storageTeams.blue && storageTeams.red > storageTeams.green) {
    ringSetAnimState(RA_RED);
  } else if (storageTeams.green > storageTeams.blue && storageTeams.green > storageTeams.red) {
    ringSetAnimState(RA_GREEN);
  } else if (storageTeams.blue > storageTeams.red && storageTeams.blue > storageTeams.green) {
    ringSetAnimState(RA_BLUE);
  } else if (storageTeams.red == storageTeams.blue && storageTeams.red == storageTeams.green) {
    ringSetAnimState(RA_RGB);
  } else if (storageTeams.red == storageTeams.green) {
    ringSetAnimState(RA_RG);
  } else if (storageTeams.red == storageTeams.blue) {
    ringSetAnimState(RA_RB);
  } else if (storageTeams.blue == storageTeams.green) {
    ringSetAnimState(RA_GB);
  }
}

void logicOnSerialMessage(const char * buf) {
  if (buf[0] == 'P') { // pairing
    logicParsePairMessage(buf);
    if (storageHasEntry(logicSeenRobotId)) {
      logValue("Robot already paired: ", logicSeenRobotId);
      ringPulseAnimOverlay(ringBlack, 2000, 0, 0.5, 4);
    } else {
      logValue("Robot paired: ", logicSeenRobotId);
      if (logicAddScore()) {
        storageAddEntry(logicSeenRobotId, logicGatePairTimeout);
        storageSaveScore();
        logicUpdateTeamCount();
        logicUpdateRingAnim();
      }
      ringPulseAnimOverlay(logicGetTeamColor(), 3000, 0, 0.5, 3); // handles both in team and not in team anim
      logicSendMqtt(MESSAGE_TYPE_PAIR);
      logicStatusTask.forceNextIteration();
    }
  }
}

void logicUpdateTeamCount() {
  Serial.write('D');
  printHexWord(storageTeams.red);
  printHexWord(storageTeams.green);
  printHexWord(storageTeams.blue);
  Serial.println();
}
