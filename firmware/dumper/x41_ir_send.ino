//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Non-portable IR send expecting IR led on pin 3 using timer2.
//

void irSendData(const uint8_t * data, const uint8_t size) {
  // we need to disable receiver because of crosstalk
  irRecvEnable(false);

  // low when not sending
  pinMode(PIN_IRTX, OUTPUT);
  digitalWrite(PIN_IRTX, LOW);

  // 38kHz freq on pin 3 using timer2
  const uint8_t pwmval = F_CPU / 2000 / 38;
  TCCR2A = _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20);
  OCR2A = pwmval;
  OCR2B = pwmval / 3;

  // header (8 slots on, 4 slots off)
  TCCR2A |= _BV(COM2B1);
  delayMicroseconds(IR_SLOT_TIME * 8 );
  TCCR2A &= ~(_BV(COM2B1));
  delayMicroseconds(IR_SLOT_TIME * 4);

  // data
  uint8_t i = size << 3;
  while (i--) {
    TCCR2A |= _BV(COM2B1);
    delayMicroseconds(IR_SLOT_TIME);
    TCCR2A &= ~(_BV(COM2B1));
    if (data[i >> 3] & (1 << (i & 7))) {
      delayMicroseconds(IR_SLOT_TIME * 3);
    } else {
      delayMicroseconds(IR_SLOT_TIME);
    }
  }

  //stop bit
  TCCR2A |= _BV(COM2B1);
  delayMicroseconds(IR_SLOT_TIME);
  TCCR2A &= ~(_BV(COM2B1));

  // switch back to input to save power
  pinMode(PIN_IRTX, INPUT);

  // enable receiver again
  irRecvEnable(true);
}

void irSendSetup() {
}
