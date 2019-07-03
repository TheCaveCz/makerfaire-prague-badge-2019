//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Animation code for info LEDS
//

Task ledsTask(100, TASK_FOREVER, &ledsCycleCb, &scheduler, true);

const uint8_t *ledsAnimCurrent;
const uint8_t *ledsAnimOriginal;

#define LEDS_ANIM_END 0b10000000
#define LEDS_ANIM_LOOP 0b01000000

void ledsCycleCb() {
  uint8_t v = 0;
  if (ledsAnimCurrent) {
    v = pgm_read_byte(ledsAnimCurrent);
    if (v & LEDS_ANIM_END) {
      ledsAnimCurrent = NULL;
    } else if (v & LEDS_ANIM_LOOP) {
      ledsAnimCurrent = ledsAnimOriginal;
    } else {
      ledsAnimCurrent++;
    }
  }
  digitalWrite(PIN_LED1, v & 1);
  digitalWrite(PIN_LED2, v & 2);
}

void ledsReset() {
  logInfo("Reset LEDs");
  digitalWrite(PIN_LED1, LOW);
  digitalWrite(PIN_LED2, LOW);
  ledsAnimCurrent = NULL;
}

void ledsSetAnim(const uint8_t * newAnim) {
  ledsAnimOriginal = ledsAnimCurrent = newAnim;
  ledsTask.forceNextIteration();
}

void ledsSetup() {
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  ledsReset();
}
