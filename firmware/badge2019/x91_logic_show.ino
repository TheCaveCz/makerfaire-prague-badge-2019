//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Showing all the seen IDs
//

void logicShowTaskCb();

Task logicShowTask(1000, TASK_FOREVER, &logicShowTaskCb, &scheduler, false);
Task logicShowButtonTask(20, TASK_FOREVER, &logicShowButtonCb, &scheduler, false);

uint16_t logicLastShownId;

uint16_t logicMyTeamId() {
  switch (storageMyTeam) {
    case STORAGE_TEAM_RED: return PIXELS_ALL_RED;
    case STORAGE_TEAM_BLUE: return PIXELS_ALL_BLUE;
    case STORAGE_TEAM_GREEN: return PIXELS_ALL_GREEN;
    default: return 0;
  }
}

void logicShowTaskMyIdCb() {
  logValue("Show ID task: ", logicShowTask.getRunCounter());

  if (logicShowTask.isLastIteration()) {
    uint16_t iter = storageSeenCount + 1; // +1 for waiting at the end (before going to sleep)
    logicShowTask.setIterations(iter);
    logicShowTask.setCallback(&logicShowTaskCb);
    logicShowTask.setInterval(iter < 8 ? 1000 : 8000 / iter); // keep fixed show time to save battery for eager collectors
    logicShowTask.restartDelayed(500);
    pixelsShowId(0);
    return;
  }

  if (logicShowTask.getRunCounter() & 1 || storageMyTeam == STORAGE_TEAM_UNDECIDED) {
    // show my id
    pixelsShowId(storageMyId);
  } else {
    // show team id
    pixelsShowId(logicMyTeamId());
  }
}

void logicShowTaskCb() {
  logValue("Show task: ", logicShowTask.getRunCounter());

  if (logicShowTask.isLastIteration()) {
    pixelsShowId(0);
    logicGoToSleep();
    return;
  }

  // find next ID to show
  // id=0 is invalid so we are OK here
  do {
    logicLastShownId++;
    if (logicLastShownId >= STORAGE_MAX_ID) {
      logInfo("ID overflow when iterating!");
      logicLastShownId = 0;
      break;
    }
  } while (!storageIdSeen(logicLastShownId));

  // and show it
  pixelsShowId(logicLastShownId);
}

void logicShowButtonCb() {
  // if pair button is pressed while showing, cancel show and start pairing
  if (buttonsRead() & BUTTON_PAIR) {
    logicShowTask.disable();
    logicShowButtonTask.disable();
    logicStartPair();
  }
}

void logicStartShow() {
  // blink the eyes
  ledsSetAnim(ledsAnimWakeupShow);

  // and prepare for the show
  logicLastShownId = 0;
  logicShowTask.setIterations(3);
  logicShowTask.setCallback(&logicShowTaskMyIdCb);
  logicShowTask.setInterval(1500);
  logicShowTask.restart();
  logicShowButtonTask.restart();
}
