//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Display dumper progress
//

#if USE_PRINTER

LiquidCrystal_I2C dispLcd(0x27, 16, 2);


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
  dispLcd.begin();
  dispLcd.createChar(0, dispCharBlock);
  dispLcd.backlight();
  dispLcd.print(F("Starting..."));
}

void dispClear() {
  dispLcd.noBacklight();
  dispLcd.clear();
}

void dispShowWelcome(const uint16_t robotId) {
  dispLcd.backlight();
  dispLcd.clear();
  dispLcd.print(F("Vidim robota"));
  dispLcd.setCursor(0, 1);
  dispLcd.write('#');
  dispLcd.print(robotId, OCT);
  dispLcd.print(F("   "));
}

void dispShowProgress(const uint8_t curr, const uint8_t max) {
  dispLcd.setCursor(0, 0);
  dispLcd.print(F("Nahravam... "));

  uint8_t p = map(curr, 0, max, 0, 12);
  dispLcd.setCursor(0, 1);
  for (uint8_t i = 0; i <= 12; i++) {
    dispLcd.write(i < p ? 0 : ' ');
  }
  p = map(curr, 0, max, 0, 100);
  dispLcd.print(p);
  dispLcd.write('%');
  if (p < 10) {
    dispLcd.write(' ');
  }
}

void dispShowResult(const uint16_t robotId, const uint16_t seen) {
  dispLcd.clear();
  dispLcd.print(F("Hotovo! "));
  dispLcd.write('#');
  dispLcd.print(robotId, OCT);

  dispLcd.setCursor(0, 1);
  dispLcd.print(F("Videl: "));
  dispLcd.print(seen);
}

void dispShowError() {
  dispLcd.clear();
  dispLcd.print(F("Chyba prenosu."));
  dispLcd.setCursor(0, 1);
  dispLcd.print(F("Zkus to znovu."));
}

#endif
