//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Ring animation helpers
//

float ringProgressNormalize(float progress) {
  while (progress > 1.0) {
    progress -= 1.0;
  }
  return progress;
}

float ringProgressRepeat(float progress, uint8_t count) {
  return count < 2 ? progress : ringProgressNormalize(progress * count);
}

float ringProgressSaw(float progress) {
  if (progress < 0.5) {
    return progress * 2.0;
  } else {
    return 2.0 - progress * 2.0;
  }
}

// maps part of progress specified by from-to to 0-1 interval, returns -1 if progress not in range
float ringProgressSlice(float progress, float from, float to) {
  if (progress < from || progress > to) return -1;

  return (progress - from) / (to - from);
}

void ringSet(const uint8_t ch, const RgbColor& c) {
  for (uint8_t i = 0; i < RING_COUNT; i++) {
    ringChannels[ch][i] = c;
  }
}

void ringSet(const uint8_t ch, uint8_t index, const RgbColor& c) {
  ringChannels[ch][index] = c;
}

RgbColor ringBlend3(const RgbColor& c1, const RgbColor& c2, const RgbColor& c3, float progress) {
  if (progress < 1.0 / 3.0) {
    return RgbColor::LinearBlend(c1, c2, NeoEase::ExponentialInOut( progress * 3.0 ));
  } else if (progress >= 2.0 / 3.0) {
    return RgbColor::LinearBlend(c3, c1, NeoEase::ExponentialInOut( progress * 3.0 - 2.0 ));
  } else {
    return RgbColor::LinearBlend(c2, c3, NeoEase::ExponentialInOut( progress * 3.0 - 1.0 ));
  }
}

RgbColor ringBlend2(const RgbColor& c1, const RgbColor& c2, float progress) {
  if (progress < 1.0 / 2.0) {
    return RgbColor::LinearBlend(c1, c2, NeoEase::ExponentialInOut( progress * 2.0 ));
  } else {
    return RgbColor::LinearBlend(c2, c1, NeoEase::ExponentialInOut( progress * 2.0 - 1.0 ));
  }
}
