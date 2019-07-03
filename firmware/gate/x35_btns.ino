//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Buttons
//

Task btnTask(300, TASK_FOREVER, &btnCb, &scheduler, true);

OneButton btn1(PIN_BTN1_IN, true);
OneButton btn2(PIN_BTN2_IN, true);
OneButton btn3(PIN_BTN3_IN, true);

uint8_t btnBlink[3];

void btnSetup() {
  pinMode(PIN_BTN1_OUT, OUTPUT);
  pinMode(PIN_BTN2_OUT, OUTPUT);
  pinMode(PIN_BTN3_OUT, OUTPUT);

  btn1.attachClick(btn1click);
  btn2.attachClick(btn2click);
  btn3.attachClick(btn3click);

  btnBlink[0] = 0;
  btnBlink[1] = 0;
  btnBlink[2] = 0;
}

void btn1click() {
#if SERIAL_COMMANDS
  Serial.println(F("!B1"));
#endif
}

void btn2click() {
#if SERIAL_COMMANDS
  Serial.println(F("!B2"));
#endif
}

void btn3click() {
#if SERIAL_COMMANDS
  Serial.println(F("!B3"));
#endif
}

void btnCb() {
  digitalWrite(PIN_BTN1_OUT, (btnBlink[0] == 2 && btnTask.getRunCounter() & 1) || (btnBlink[0] == 1));
  digitalWrite(PIN_BTN2_OUT, (btnBlink[1] == 2 && btnTask.getRunCounter() & 1) || (btnBlink[1] == 1));
  digitalWrite(PIN_BTN3_OUT, (btnBlink[2] == 2 && btnTask.getRunCounter() & 1) || (btnBlink[2] == 1));
}

void btnTick() {
  btn1.tick();
  btn2.tick();
  btn3.tick();
}
