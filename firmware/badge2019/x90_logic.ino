//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Basic functions for badge behavior.
//

bool logicNeedSleep;
bool logicForceAwake;

Task logicForceAwakeTask(1, 1, &logicForceAwakeCb, &scheduler, false);

void logicSetup() {
  logicForceAwake = true;
  logicNeedSleep = false;
  logicForceAwakeTask.restartDelayed(2500);
}

void logicForceAwakeCb() {
  logInfo("Disabling force awake");
  logicForceAwake = false;
}

void logicSetForceAwake(const bool a) {
  logicForceAwake = a;
  if (a) {
    logicForceAwakeTask.restartDelayed(10000);
  } else {
    logicForceAwakeTask.disable();
  }
}

// toggle flag for sleep mode - doesn't sleep immediately but on next scheduler cycle
void logicGoToSleep() {
  logicNeedSleep = true;
}

// go to sleep if requested and after wake-up start the right action
void logicCheckSleep() {
  if (!logicNeedSleep) return;
  if (logicForceAwake) return;

  uint8_t res = powerEnterSleep();
  logicNeedSleep = false;
  if (res & BUTTON_PAIR) { // pair button
    logicStartPair();
  } else if (res & BUTTON_SHOW) { // show button
    logicStartShow();
  }
}
