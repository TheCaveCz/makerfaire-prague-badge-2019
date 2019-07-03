//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Storing dumped IDs
//

#define STORAGE_MAX_ID 4096
#define STORAGE_COUNT 512
#define STORAGE_TREASURE_COUNT 30
#define STORAGE_GATE_COUNT 6

const uint16_t storageTreasureIds[STORAGE_TREASURE_COUNT] PROGMEM = {
  3466,
  3467,
  3468,
  3469,
  3471,
  3473,
  3475,
  3476,
  3477,
  3479,
  3481,
  3482,
  3484,
  3485,
  3487,
  3489,
  3490,
  3491,
  3493,
  3495,
  3497,
  3498,
  3499,
  3500,
  3503,
  3513,
  3514,
  3515,
  3516,
  3517,
};

const uint16_t storageGateIds[STORAGE_GATE_COUNT] PROGMEM = {
  1740,
  1745,
  1765,
  1767,
  1778,
  1789,
};

uint8_t currentDump[STORAGE_COUNT];

bool storageIdValid(uint16_t id) {
  return ((id & 0b111) != 0) &&
         ((id & 0b111000) != 0) &&
         ((id & 0b111000000) != 0) &&
         ((id & 0b111000000000) != 0) &&
         (id < STORAGE_MAX_ID);
}

bool storageIdSeen(uint16_t id) {
  if (id >= STORAGE_MAX_ID) return 0;

  return currentDump[id >> 3] & (1 << (id & 0b111)) ? 1 : 0;
}

bool storageTreasureSeen(uint8_t id) {
  return storageIdSeen(pgm_read_word(storageTreasureIds + id));
}

bool storageGateSeen(uint8_t id) {
  return storageIdSeen(pgm_read_word(storageGateIds + id));
}

uint8_t storageCalcTreasureSeen() {
  uint8_t result = 0;
  for (uint8_t i = 0; i < STORAGE_TREASURE_COUNT; i++) {
    if (storageTreasureSeen(i)) result++;
  }
  return result;
}

void storageResetDump() {
  memset(currentDump, 0, sizeof(currentDump));
}

uint16_t storageCalcSeenCount() {
  uint16_t result = 0;
  for (uint16_t i = 0; i < STORAGE_COUNT; i++) {
    uint8_t b = currentDump[i];
    // count number of set bits in b
    for (uint8_t j = 0; j < 8; j++) {
      if (b & (1 << j)) result++;
    }
  }
  return result;
}

void storageSaveSeenByte(const uint16_t addr, uint8_t data) {
  if (addr < STORAGE_COUNT) currentDump[addr] = data;
}
