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
#define STORAGE_MAX_ID 4096

#define STORAGE_MAX_TEAM 4
#define STORAGE_TEAM_UNDECIDED 0
#define STORAGE_TEAM_RED 1
#define STORAGE_TEAM_GREEN 2
#define STORAGE_TEAM_BLUE 3

// EEPROM map
// 0-1 myId
#define STORAGE_EEPROM_MYID 0

// each ID is up to 12 bits in size. If we store only the seen/not-seen info (1bit), this needs 4096 bits => 512 bytes in EEPROM
uint16_t storageMyId;


// ID is only valid when any of 3bit pairs of the ID is not zero
bool storageIdValid(uint16_t id) {
  return ((id & 0b111) != 0) &&
         ((id & 0b111000) != 0) &&
         ((id & 0b111000000) != 0) &&
         ((id & 0b111000000000) != 0) &&
         (id < STORAGE_MAX_ID);
}

bool storageSetId(const uint16_t id) {
  if (!storageIdValid(id)) return false;

  storageMyId = id;
  logValue("New id set: ", storageMyId);
  EEPROM.put(STORAGE_EEPROM_MYID, storageMyId);
  return true;
}

void storageSetup() {
  EEPROM.get(STORAGE_EEPROM_MYID, storageMyId);
  logValue("My ID ", storageMyId);
  if (!storageIdValid(storageMyId)) {
    logInfo("Oh-noes, invalid own ID");
  }
}
