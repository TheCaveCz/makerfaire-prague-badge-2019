//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Storing seen IDs.
//

#define STORAGE_MAGIC 0x5b
#define STORAGE_MAGIC2 42
#define STORAGE_COUNT 512
#define STORAGE_MAX_ID 4096
#define STORAGE_WILDCARD_ID (STORAGE_MAX_ID-1)

#define STORAGE_MAX_TEAM 4
#define STORAGE_TEAM_UNDECIDED 0
#define STORAGE_TEAM_RED 1
#define STORAGE_TEAM_GREEN 2
#define STORAGE_TEAM_BLUE 3

// EEPROM map
// 0-1 myId
// 2 teamId
// 3 magic1
// 4-515 seen ids
// 516 magic2
#define STORAGE_EEPROM_MYID 0
#define STORAGE_EEPROM_TEAMID 2
#define STORAGE_EEPROM_MAGIC1 3
#define STORAGE_EEPROM_SEEN_IDS 4
#define STORAGE_EEPROM_MAGIC2 516

// each ID is up to 12 bits in size. If we store only the seen/not-seen info (1bit), this needs 4096 bits => 512 bytes in EEPROM
uint8_t storageSeenIds[STORAGE_COUNT];
uint16_t storageSeenCount;
uint16_t storageMyId;
uint8_t storageMyTeam;


uint16_t storageCalcSeenCount() {
  uint16_t result = 0;
  for (uint16_t i = 0; i < STORAGE_COUNT; i++) {
    uint8_t b = storageSeenIds[i];
    // count number of set bits in b
    for (uint8_t j = 0; j < 8; j++) {
      if (b & (1 << j)) result++;
    }
  }
  return result;
}

uint8_t storageGetSeenByte(const uint16_t addr) {
  return addr < STORAGE_COUNT ? storageSeenIds[addr] : 0;
}


// ID is only valid when any of 3bit pairs of the ID is not zero
bool storageIdValid(uint16_t id) {
  return ((id & 0b111) != 0) &&
         ((id & 0b111000) != 0) &&
         ((id & 0b111000000) != 0) &&
         ((id & 0b111000000000) != 0) &&
         (id < STORAGE_MAX_ID);
}

bool storageIdSeen(uint16_t id) {
  if (id >= STORAGE_MAX_ID) return 0;

  return storageSeenIds[id >> 3] & (1 << (id & 0b111)) ? 1 : 0;
}

void storageDebugDump() {
#if LOG_ENABLED
  logValue("Seen ids count: ", storageSeenCount);
  //  logRaw(F("---DUMP FOLLOWS"));
  //  logLine();
  //  for (uint16_t i = 0; i < STORAGE_COUNT; i++) {
  //    if (storageSeenIds[i] < 16) logRaw('0');
  //    logNum(storageSeenIds[i], HEX);
  //    if (i % 16 == 15) {
  //      logLine();
  //    } else {
  //      logRaw(' ');
  //    }
  //  }
  //  logLine();

  //  uint8_t cnt = 0;
  //  for (uint16_t id = 0; id < STORAGE_MAX_ID; id++) {
  //    if (!storageIdSeen(id)) continue;
  //
  //    logRaw(id);
  //    if (cnt < 16) {
  //      cnt++;
  //      logRaw(',');
  //    } else {
  //      cnt = 0;
  //      logLine();
  //    }
  //  }
  //  logLine();
#endif
}


void storageMarkIdSeen(uint16_t id) {
  logValue("Marking id seen: ", id);

  if (id >= STORAGE_MAX_ID) return;

  uint16_t offset = id >> 3;
  storageSeenIds[offset] |= (1 << (id & 0b111));
  EEPROM.write(STORAGE_EEPROM_SEEN_IDS + offset, storageSeenIds[offset]);

  storageSeenCount = storageCalcSeenCount();
  storageDebugDump();
}

bool storageSetTeam(const uint8_t team) {
  if (team >= STORAGE_MAX_TEAM) return false;

  storageMyTeam = team;
  logValue("New team id set: ", storageMyTeam);
  EEPROM.write(STORAGE_EEPROM_TEAMID, storageMyTeam);
  return true;
}

bool storageSetId(const uint16_t id) {
  if (!storageIdValid(id)) return false;

  storageMyId = id;
  logValue("New id set: ", storageMyId);
  EEPROM.put(STORAGE_EEPROM_MYID, storageMyId);
  return true;
}

void storageFormat() {
  logInfo("Formatting storage");
  memset(storageSeenIds, 0, sizeof(storageSeenIds));
  EEPROM.put(STORAGE_EEPROM_SEEN_IDS, storageSeenIds);
  EEPROM.write(STORAGE_EEPROM_MAGIC1, STORAGE_MAGIC);
  EEPROM.write(STORAGE_EEPROM_MAGIC2, STORAGE_MAGIC2);
  storageSeenCount = 0;
}

void storageSetup() {
  EEPROM.get(STORAGE_EEPROM_MYID, storageMyId);
  logValue("My ID ", storageMyId);
  if (!storageIdValid(storageMyId)) {
    logInfo("Oh-noes, invalid own ID");
  }

  storageMyTeam = EEPROM.read(STORAGE_EEPROM_TEAMID);
  if (storageMyTeam >= STORAGE_MAX_TEAM) {
    logInfo("On-noes, invalid team ID");
    storageMyTeam = STORAGE_TEAM_UNDECIDED;
    EEPROM.write(STORAGE_EEPROM_TEAMID, storageMyTeam);
  }
  logValue("My team ", storageMyTeam);

  uint8_t magic = EEPROM.read(STORAGE_EEPROM_MAGIC1);
  uint8_t magic2 = EEPROM.read(STORAGE_EEPROM_MAGIC2);
  if (magic == STORAGE_MAGIC && magic2 == STORAGE_MAGIC2) {
    logInfo("Storage loaded");

    EEPROM.get(STORAGE_EEPROM_SEEN_IDS, storageSeenIds);
  } else {
    storageFormat();
  }

  storageSeenCount = storageCalcSeenCount();
  storageDebugDump();
}
