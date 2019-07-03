//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// 7seg displays handling 
//

TM1637Display dispRed(PIN_D1_CLK, PIN_D1_DIO);
TM1637Display dispGreen(PIN_D2_CLK, PIN_D2_DIO);
TM1637Display dispBlue(PIN_D3_CLK, PIN_D3_DIO);

uint16_t dispValues[3];

void dispUpdate() {
  dispRed.showNumberDec(dispValues[0], false);
  dispGreen.showNumberDec(dispValues[1], false);
  dispBlue.showNumberDec(dispValues[2], false);
}

void dispSetup() {
  memset(dispValues, 0, sizeof(dispValues));
  dispRed.setBrightness(0x0f);
  dispGreen.setBrightness(0x0f);
  dispBlue.setBrightness(0x0f);
  dispUpdate();
}
