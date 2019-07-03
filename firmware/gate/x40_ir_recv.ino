//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Handling of infrared receiver. This code is using interrupts and is very timing sensitive.
//

#define IR_RECV_MAX_CMDS 8
#define IR_RECV_CMDS_MASK 0b111

#define IR_STATE_DISABLED 0
#define IR_STATE_H0 1
#define IR_STATE_D1 2
#define IR_STATE_D0 3
#define IR_STATE_INVALID 4

#define IR_SLOT_TIME 282

#define IR_MSG_LEN 5
#define IR_RECV_SLOT_MIN ((IR_SLOT_TIME>>6) - 1)
#define IR_RECV_SLOT_MAX ((IR_SLOT_TIME>>6) + 3)

#define IR_RECV_CMD_INVALID 0
#define IR_RECV_CMD_NEW 1

volatile uint32_t irRecvLastTime;
volatile uint8_t irRecvState;
uint8_t irRecvIsrData[IR_MSG_LEN];
volatile uint8_t irRecvIsrCount;

// circular buffer for received commands
uint8_t irRecvCommands[IR_RECV_MAX_CMDS][IR_MSG_LEN];
volatile uint8_t irRecvCommandsEnd;
volatile uint8_t irRecvCommandsBegin;



void irRecvIsr() {
  if (irRecvState == IR_STATE_DISABLED) return;

  uint32_t t = micros();

  // Since we are decoding 282us pulses we can afford to scale the values down to multiples of 64 microseconds.
  // This gives us enough precision and tolerance to decode it.
  uint32_t t1 = (t - irRecvLastTime) >> 6;
  irRecvLastTime = t;

  if (t1 > 255) {
    // if the pulse is too long ignore it to avoid overflow
    irRecvState = IR_STATE_INVALID;
    return;
  }

  uint8_t delta = t1;

  // Normally sensor is active low (when receiving carrier freq. it outputs low) so the actual received data are inverted.
  // But we always measure time since change and this means we need to invert the value again to check previous state.
  // No need for double invert, we will just use original value and we are ok.
  // In IR code we commonly use word MARK for 1, SPACE for 0
  if (digitalRead(PIN_IRRX)) {
    // processing marks

    if (delta >= IR_RECV_SLOT_MIN * 8  && delta <= IR_RECV_SLOT_MAX * 8) {
      // mark for 8 slots => this might be header
      irRecvState = IR_STATE_H0;

    } else if (irRecvState == IR_STATE_D1 && delta >= IR_RECV_SLOT_MIN && delta <= IR_RECV_SLOT_MAX) {
      // mark for 1 slot
      if (irRecvIsrCount == 0) {
        // if we have enough bits this is stop bit => finish decoding
        irRecvState = IR_STATE_INVALID;
        memcpy(&(irRecvCommands[irRecvCommandsBegin][0]), irRecvIsrData, IR_MSG_LEN);
        irRecvCommandsBegin++;
        irRecvCommandsBegin &= IR_RECV_CMDS_MASK;
        if (irRecvCommandsBegin == irRecvCommandsEnd) {
          irRecvCommandsEnd++;
          irRecvCommandsEnd &= IR_RECV_CMDS_MASK;
        }

      } else {
        // not enough bits yet? decrease bit counter and expect more
        irRecvState = IR_STATE_D0;
        irRecvIsrCount--;
      }

    } else {
      // jump into invalid state if there is no match for current state and delta
      // only header mark can pull out of invalid state

      irRecvState = IR_STATE_INVALID;
    }


  } else {
    // processing spaces

    if (irRecvState == IR_STATE_H0 && delta >= IR_RECV_SLOT_MIN * 4 && delta <= IR_RECV_SLOT_MAX * 4) {
      // space for 4 slots => this is definitely header => reset state and prepare for decoding
      irRecvState = IR_STATE_D1;
      memset(irRecvIsrData, 0, sizeof(irRecvIsrData));
      irRecvIsrCount = IR_MSG_LEN * 8;

    } else if (irRecvState == IR_STATE_D0 && delta >= IR_RECV_SLOT_MIN * 3 && delta <= IR_RECV_SLOT_MAX * 3) {
      // space for 3 slots => this bit is ONE
      irRecvState = IR_STATE_D1;
      irRecvIsrData[irRecvIsrCount >> 3] |= (1 << (irRecvIsrCount & 7));

    } else if (irRecvState == IR_STATE_D0 && delta >= IR_RECV_SLOT_MIN && delta <= IR_RECV_SLOT_MAX) {
      // space for 1 slot => this bit is ZERO
      irRecvState = IR_STATE_D1;

    } else {
      // jump into invalid state if there is no match for current state and delta
      // only header mark can pull out of invalid state

      irRecvState = IR_STATE_INVALID;
    }
  }
}

// returns 0 for no command, 1 for new command, 2 for old command
bool irRecvGetCommand(void * p) {
  uint8_t result = false;
  noInterrupts();
  if (irRecvCommandsBegin != irRecvCommandsEnd) {
    result = true;
    memcpy(p, &(irRecvCommands[irRecvCommandsEnd][0]), IR_MSG_LEN);
    irRecvCommandsEnd++;
    irRecvCommandsEnd &= IR_RECV_CMDS_MASK;
  }
  interrupts();
  return result;
}


void irRecvCommandsClear() {
  noInterrupts();
  irRecvCommandsEnd = irRecvCommandsBegin;
  irRecvState = IR_STATE_INVALID;
  interrupts();
}

void irRecvEnable(const bool e) {
  irRecvState = e ? IR_STATE_INVALID : IR_STATE_DISABLED;
}

void irRecvSetup() {
  pinMode(PIN_IRRX, INPUT);

  memset(irRecvCommands, 0, sizeof(irRecvCommands));
  irRecvCommandsEnd = 0;
  irRecvCommandsBegin = 0;

  irRecvState = IR_STATE_INVALID;
  irRecvLastTime = micros();
  attachInterrupt(digitalPinToInterrupt(PIN_IRRX), irRecvIsr, CHANGE);
}
