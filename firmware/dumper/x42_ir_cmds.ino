//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Higher level handling of IR communication.
//


#define IR_CMD_FLAG 0x30

#define IR_CMD_PRESENCE (IR_CMD_FLAG | 1)
#define IR_CMD_PAIR_ACK (IR_CMD_FLAG | 2)
#define IR_CMD_PAIR_CONFIRM (IR_CMD_FLAG | 3)
#define IR_CMD_DUMP_REQUEST (IR_CMD_FLAG | 4)
#define IR_CMD_DUMP_ACK (IR_CMD_FLAG | 5)
#define IR_CMD_DUMP_INFO (IR_CMD_FLAG | 6)
#define IR_CMD_PRESENCE_OLD (IR_CMD_FLAG | 7)
#define IR_CMD_CONFIRM_OLD (IR_CMD_FLAG | 8)
#define IR_CMD_TEAM_SET (IR_CMD_FLAG | 9)
#define IR_CMD_TEAM_CONFIRM (IR_CMD_FLAG | 10)



union IrCommand {
  struct Fields {
    uint8_t teamId: 2;
    uint8_t cmd: 6; // effectively 4bits because IR_CMD_FLAG is always present
    uint16_t id1: 12;
    uint16_t id2: 12;
    uint8_t crc; // must be last item
  } fields;
  struct Dump {
    uint8_t pageId;
    uint8_t data[3];
    uint8_t crc;
  } dump;
  uint8_t bytes[IR_MSG_LEN];
};


// modes for dumping IR commands
#define IR_CMD_DUMP_MODE_SEND 0
#define IR_CMD_DUMP_MODE_RECV 1


// Dow-CRC using polynomial X^8 + X^5 + X^4 + X^0
// Tiny 2x16 entry CRC table created by Arjen Lentz
// See http://lentz.com.au/blog/calculating-crc-with-a-tiny-32-entry-lookup-table
static const uint8_t PROGMEM dscrc2x16_table[] = {
  0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83,
  0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
  0x00, 0x9D, 0x23, 0xBE, 0x46, 0xDB, 0x65, 0xF8,
  0x8C, 0x11, 0xAF, 0x32, 0xCA, 0x57, 0xE9, 0x74
};

// Compute a Dallas Semiconductor 8 bit CRC. These show up in the ROM
// and the registers.  (Use tiny 2x16 entry CRC table)
uint8_t crc8(const uint8_t *addr, uint8_t len) {
  uint8_t crc = 0;

  while (len--) {
    crc = *addr++ ^ crc;  // just re-using crc as intermediate
    crc = pgm_read_byte(dscrc2x16_table + (crc & 0x0f)) ^
          pgm_read_byte(dscrc2x16_table + 16 + ((crc >> 4) & 0x0f));
  }

  return crc;
}

void irCommandDebug(union IrCommand& c, uint8_t mode) {
#if LOG_ENABLED
  logHeader();

  //  for (uint8_t i = 0; i < 5; i++) {
  //    logNum(c.bytes[i], HEX);
  //    logRaw(',');
  //  }

  if ((c.fields.cmd & IR_CMD_FLAG) == IR_CMD_FLAG) {
    logRaw(mode == IR_CMD_DUMP_MODE_RECV ? F("Received IR command 0x") : F("Sent IR command 0x"));
    logNum(c.fields.cmd, HEX);
    logRaw(',');
    logRaw(c.fields.teamId);
    logRaw(',');
    logRaw(c.fields.id1);
    logRaw(',');
    logRaw(c.fields.id2);
  } else {
    logRaw(mode == IR_CMD_DUMP_MODE_RECV ? F("Received IR dump ") : F("Sent IR dump ") );
    logRaw(c.dump.pageId);
    logRaw(',');
    logNum(c.dump.data[0], HEX);
    logRaw(',');
    logNum(c.dump.data[1], HEX);
    logRaw(',');
    logNum(c.dump.data[2], HEX);
  }

  logLine();
#endif
}

bool irCommandReceived(IrCommand& c) {
  if (irRecvGetCommand(c.bytes)) {
    irCommandDebug(c, IR_CMD_DUMP_MODE_RECV);
    return c.fields.crc == crc8(c.bytes, IR_MSG_LEN - 1);
  }

  return false;
}

void irCommandSendRaw(IrCommand& c) {
  c.fields.crc = crc8(c.bytes, IR_MSG_LEN - 1);

  irCommandDebug(c, IR_CMD_DUMP_MODE_SEND);

  irSendData(c.bytes, IR_MSG_LEN);
}

void irCommandSend(const uint8_t cmd, const uint8_t team, const uint16_t id1, const uint16_t id2) {
  IrCommand c;
  c.fields.cmd = cmd;
  c.fields.teamId = team;
  c.fields.id1 = id1;
  c.fields.id2 = id2;
  irCommandSendRaw(c);
}

void irCommandSendDump(const uint8_t pageId, const uint8_t b1, const uint8_t b2, const uint8_t b3) {
  IrCommand c;
  c.dump.pageId = pageId;
  c.dump.data[0] = b1;
  c.dump.data[1] = b2;
  c.dump.data[2] = b3;
  irCommandSendRaw(c);
}
