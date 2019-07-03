//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Button handling
//

#define BUTTON_PAIR 1
#define BUTTON_SHOW 2

void buttonsSetup() {
  // enable pullups for wakeup buttons
  pinMode(PIN_SW1, INPUT_PULLUP);
  pinMode(PIN_SW2, INPUT_PULLUP);

}

uint8_t buttonsRead() {
  uint8_t res = 0;
  if (digitalRead(PIN_SW1) == LOW) res |= BUTTON_PAIR;
  if (digitalRead(PIN_SW2) == LOW) res |= BUTTON_SHOW;
  return res;
}
