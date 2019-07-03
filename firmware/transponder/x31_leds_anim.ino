//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Animation data for info LEDS
//

const uint8_t ledsAnimAcknowledge[] PROGMEM = {
  3,
  0, 0, 0 | LEDS_ANIM_LOOP
};

const uint8_t ledsAnimDuplicate[] PROGMEM = {
  0,
  3, 3, 3 | LEDS_ANIM_LOOP
};

const uint8_t ledsAnimDump[] PROGMEM = {
  1, 1,
  3, 3,
  2, 2,
  3, 3 | LEDS_ANIM_LOOP
};
