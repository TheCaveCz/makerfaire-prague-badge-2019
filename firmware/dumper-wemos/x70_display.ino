//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Handling OLED display 
//

Task displayTask(250, TASK_FOREVER, &displayCb, &scheduler, true);

Adafruit_SSD1306 display;

void displaySetup() {
  display.begin();
  display.setRotation(3);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.clearDisplay();
  display.print("Boot");
  display.display();
}

void displayCb() {
  display.clearDisplay();

  display.setCursor(0, 0);
  if (logicDumpStarted) {
    display.print("D #");
    display.print(logicDump.robotId);

    display.setCursor(0, 10);
    display.print(logicCurrentPage);
    display.write('/');
    display.print(logicPagesCount);

    display.drawRect(0, 18, display.width(), 8, WHITE);
    display.fillRect(2, 18 + 2, map(logicCurrentPage, 0, logicPagesCount, 0, display.width() - 4), 4, WHITE);
  } else {
    display.print("Ready");
  }

  display.setCursor(0, 36);
  display.print(logicDumpsCount);
  display.write('/');
  display.print(sendingCount());

  display.setCursor(0, 48);
  if (WiFi.isConnected()) {
    display.print("WiFi:OK");
  } else {
    display.print("WiFi:no");
  }

  display.setCursor(0, 56);
  if (mqttClient.connected()) {
    display.print("MQTT:OK");
  } else {
    display.print("MQTT:no");
  }
  display.display();
}
