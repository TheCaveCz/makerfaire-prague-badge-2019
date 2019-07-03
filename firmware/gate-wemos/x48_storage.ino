//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Storage for seen IDs
//

#define STORAGE_MAX 500

Task storageTask(60000, TASK_FOREVER, &storageSaveCb, &scheduler, true);
Task storageAgeTask(1000, TASK_FOREVER, &storageAgeEntriesCb, &scheduler, true);

struct StorageTeams {
  uint16_t red;
  uint16_t blue;
  uint16_t green;
} storageTeams;

bool storageEntriesDirty;

struct __attribute__((__packed__)) StorageEntry {
  uint16_t id: 12;
  uint16_t remaining: 12;
} storageEntries[STORAGE_MAX];

void storageRead(File& f, uint8_t* data, size_t size) {
  size_t r = f.read(data, size);
  if (size != r) {
    logValue("Read mismatch, only ", r);
  }
}

bool storageWrite(File& f, const uint8_t* data, size_t size) {
  size_t r = f.write(data, size);
  if (size != r) {
    logValue("Write mismatch, only ", r);
    return false;
  }
  return true;
}

void storageAgeEntriesCb() {
  for (uint16_t i = 0; i < STORAGE_MAX; i++) {
    if (!storageEntries[i].id) continue;

    if (storageEntries[i].remaining) {
      storageEntries[i].remaining--;
      storageEntriesDirty = true;
    }

    if (!storageEntries[i].remaining && storageEntries[i].id) {
      storageEntries[i].id = 0;
      storageEntriesDirty = true;
    }
  }
}

int storageFindEntry(const uint16_t id) {
  for (uint16_t i = 0; i < STORAGE_MAX; i++) {
    if (storageEntries[i].id == id) return i;
  }
  return -1;
}

bool storageHasEntry(const uint16_t id) {
  return storageFindEntry(id) != -1;
}

bool storageAddEntry(const uint16_t id, const uint16_t time) {
  if (id == 0) return false;

  int index = storageFindEntry(id);
  if (index < 0) {
    index = storageFindEntry(0);
    if (index < 0) {
      return false;
    }
  }
  storageEntries[index].id = id;
  storageEntries[index].remaining = time;
  storageEntriesDirty = true;
  logValue("Added entry ", id);
  return true;
}

bool storageRemoveEntry(const uint16_t id) {
  if (id == 0) return false;
  int index = storageFindEntry(id);
  if (index < 0) return false;

  storageEntries[index].id = 0;
  storageEntries[index].remaining = 0;
  storageEntriesDirty = true;
  logValue("Removed entry ", id);
  return true;
}

uint16_t storageCountEntries() {
  uint16_t result = 0;
  for (uint16_t i = 0; i < STORAGE_MAX; i++) {
    if (storageEntries[i].id) result++;
  }
  return result;
}

void storageSaveCb() {
  if (storageEntriesDirty && storageSaveEntries()) {
    storageEntriesDirty = false;
    logInfo("Saved entries");
  }
}

void storageSetup() {
  if (!SPIFFS.begin()) {
    logInfo("Failed to start spiffs");
  }

  logValue("Size of entries: ", sizeof(storageEntries));

  memset(&storageTeams, 0, sizeof(StorageTeams));
  memset(&storageEntries, 0, sizeof(storageEntries));

  File f = SPIFFS.open("/score", "r");
  if (f) {
    storageRead(f, (uint8_t*)&storageTeams, sizeof(StorageTeams));
    f.close();
  } else {
    logInfo("Failed to open score file");
    storageSaveScore();
  }

  f = SPIFFS.open("/entries", "r");
  if (f) {
    storageRead(f, (uint8_t*)storageEntries, sizeof(storageEntries));
    f.close();
  } else {
    logInfo("Failed to open entries file");
    storageSaveEntries();
  }
}

bool storageSaveScore() {
  File f = SPIFFS.open("/score", "w");
  if (!f) {
    logInfo("Failed to write score file");
    return false;
  }

  bool result = storageWrite(f, (uint8_t*)&storageTeams, sizeof(StorageTeams));
  f.close();
  return result;
}

bool storageSaveEntries() {
  File f = SPIFFS.open("/entries", "w");
  if (!f) {
    logInfo("Failed to write entries file");
    return false;
  }

  bool result = storageWrite(f, (uint8_t*)storageEntries, sizeof(storageEntries));
  f.close();
  return result;
}
