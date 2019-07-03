//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Team count storage
//

struct StorageTeams {
  uint16_t red;
  uint16_t blue;
  uint16_t green;
} storageTeams;

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


void storageSetup() {
  if (!SPIFFS.begin()) {
    logInfo("Failed to start spiffs");
  }

  memset(&storageTeams, 0, sizeof(StorageTeams));

  File f = SPIFFS.open("/score", "r");
  if (f) {
    storageRead(f, (uint8_t*)&storageTeams, sizeof(StorageTeams));
    f.close();
  } else {
    logInfo("Failed to open score file");
    storageSaveScore();
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
