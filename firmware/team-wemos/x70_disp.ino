//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Handling LCD display
//

Task dispTask(200, TASK_FOREVER, &dispCb, &scheduler, true);
LiquidCrystal_I2C disp(0x27, 20, 4);

void dispPrintTeam(LogicTeam t);
void dispPrintTeamEn(LogicTeam t);

byte dispCharWifi[] = {
  B01010,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00100,
  B00100
};

byte dispCharBlock[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

void dispSetup() {
  disp.begin();
  disp.createChar(0, dispCharWifi);
  disp.createChar(1, dispCharBlock);
  disp.backlight();
}

void dispPrintTeam(LogicTeam t) {
  switch (t) {
    case LT_RED: disp.print("Cerveny"); break;
    case LT_GREEN: disp.print("Zeleny"); break;
    case LT_BLUE: disp.print("Modry"); break;
    default: disp.print("Zadny tym"); break;
  }
}

void dispPrintTeamEn(LogicTeam t) {
  switch (t) {
    case LT_RED: disp.print("Red"); break;
    case LT_GREEN: disp.print("Green"); break;
    case LT_BLUE: disp.print("Blue"); break;
    default: disp.print("No team"); break;
  }
}

void dispPrintIndicator() {
  disp.setCursor(19, 0);
  disp.write(WiFi.isConnected() ? (mqttClient.connected() ? ' ' : '-') : 0);
}

void dispPrintTimeout() {
  uint8_t p = map(logicTimeout.getIterations(), 0, logicTimeout.getIterations() + logicTimeout.getRunCounter(), 0, 16);
  disp.setCursor(0, 3);
  for (uint8_t i = 0; i <= 16; i++) {
    disp.write(i < p ? 1 : ' ');
  }
  p = logicTimeout.getIterations() / 10 + 1;
  disp.print(p);
  disp.write('s');
  if (p < 10) {
    disp.write(' ');
  }
}

void dispClear() {
  disp.clear();
}

void dispCb() {
  switch (logicState) {
    case LS_READY:
      disp.setCursor(0, 0);
      disp.print("Vyber svuj tym");
      disp.setCursor(0, 1);
      disp.print("stiskem tlacitka!");
      disp.setCursor(0, 2);
      disp.print("Pick your team by");
      disp.setCursor(0, 3);
      disp.print("pressing a button!");
      dispPrintIndicator();
      break;

    case LS_PAIR_INFO:
      disp.setCursor(0, 0);
      disp.print("Robot #");
      disp.print(logicSeenRobotId, OCT);

      disp.setCursor(0, 1);
      dispPrintTeam(logicSeenRobotTeam);
      disp.write('/');
      dispPrintTeamEn(logicSeenRobotTeam);

      disp.setCursor(0, 2);
      disp.print("Videl/Seen: ");
      disp.print(logicSeenRobotCount);

      dispPrintTimeout();
      break;

    case LS_TEAM_FULL:
      disp.setCursor(0, 0);
      dispPrintTeam(logicJoiningTeam);
      disp.print(" tym je plny!");
      disp.setCursor(0, 1);
      dispPrintTeamEn(logicJoiningTeam);
      disp.print(" team is full!");

      dispPrintTimeout();
      break;

    case LS_TRY_AGAIN:
      disp.setCursor(0, 0);
      disp.print("Chyba parovani, zkus");
      disp.setCursor(0, 1);
      disp.print("to znovu! / Pair ");

      disp.setCursor(0, 2);
      disp.print("error, try again!");

      dispPrintTimeout();
      break;

    case LS_PAIR_REQUEST:
      disp.setCursor(0, 0);
      disp.print("Sparuj robota!");
      disp.setCursor(0, 1);
      disp.print("Pair your robot now!");

      dispPrintTimeout();
      break;

    case LS_ALREADY_TEAMED:
      disp.setCursor(0, 0);
      disp.print("Robot uz je v tymu!");

      disp.setCursor(0, 1);
      disp.print("Already has a team!");

      disp.setCursor(0, 2);
      dispPrintTeam(logicSeenRobotTeam);
      disp.write('/');
      dispPrintTeamEn(logicSeenRobotTeam);

      dispPrintTimeout();
      break;

    case LS_PAIR_SUCCESS:
      disp.setCursor(0, 0);
      disp.print("Hotovo! Tym: ");
      dispPrintTeam(logicSeenRobotTeam);

      disp.setCursor(0, 1);
      disp.print("Done! Team: ");
      dispPrintTeamEn(logicSeenRobotTeam);

      dispPrintTimeout();
      break;

    default: break;
  }
}
