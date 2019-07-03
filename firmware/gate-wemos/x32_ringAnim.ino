//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Ring animation implementation
//

// this is blendable animation
void ringAllContestedAnim(const uint8_t ch) {
  ringAnimations.StartAnimation(ch, 20000, [ = ](const AnimationParam & param) {
    for (uint8_t i = 0; i < RING_COUNT; i++) {
      float p = ringProgressNormalize( ((float)i / RING_COUNT) + param.progress );
      ringSet(ch, i, ringBlend3(ringRed, ringGreen, ringBlue, p));
    }

    if (param.state == AnimationState_Completed)  {
      ringAnimations.RestartAnimation(param.index);
    }
  });
}

// this is blendable animation
void ringTwoContestedAnim(const uint8_t ch, const RgbColor& c1, const RgbColor& c2) {
  ringAnimations.StartAnimation(ch, 20000, [ = ](const AnimationParam & param) {
    for (uint8_t i = 0; i < RING_COUNT; i++) {
      float p = ringProgressNormalize( ((float)i / RING_COUNT) + param.progress );
      ringSet(ch, i, ringBlend2(c1, c2, p));
    }

    if (param.state == AnimationState_Completed)  {
      ringAnimations.RestartAnimation(param.index);
    }
  });
}

// this is blendable animation
void ringPulseAnim(const uint8_t ch, const RgbColor& c) {
  ringPulseAnim(ch, c, -64, 10000, 0.5, 0.6);
}

// this is blendable animation
void ringPulseAnim(const uint8_t ch, const RgbColor& c, int amount, int time, float from, float to) {
  RgbColor cd = RgbColor(c);
  if (amount > 0) {
    cd.Lighten(amount);
  } else {
    cd.Darken(-amount);
  }
  ringPulseAnim(ch, c, cd, time, from, to);
}

// this is blendable animation
void ringPulseAnim(const uint8_t ch, const RgbColor& c1, const RgbColor& c2, int time, float from, float to) {
  ringAnimations.StartAnimation(ch, time, [ = ](const AnimationParam & param) {
    float p = param.progress;
    p = ringProgressSlice(p, from, to);
    if (p >= 0) {
      ringSet(ch, RgbColor::LinearBlend(c1, c2, NeoEase::CubicIn(ringProgressSaw(p))));
    } else {
      ringSet(ch, c1);
    }

    if (param.state == AnimationState_Completed)  {
      ringAnimations.RestartAnimation(param.index);
    }
  });
}

// this is overlay animation - runs in separate channel (after blender)
void ringPulseAnimOverlay(const RgbColor& c2, int time, float from, float to, uint8_t repeats) {
  ringAnimations.StartAnimation(3, time, [ = ](const AnimationParam & param) {
    float p = ringProgressRepeat(param.progress, repeats);
    p = ringProgressSlice(p, from, to);
    if (p >= 0) {
      for (uint8_t i = 0; i < RING_COUNT; i++) {
        ringLeds.SetPixelColor(i, RgbColor::LinearBlend(ringLeds.GetPixelColor(i), ringGamma.Correct(c2), NeoEase::ExponentialInOut(ringProgressSaw(p))));
      }
    }

    if (param.state == AnimationState_Completed && repeats == 0)  {
      ringAnimations.RestartAnimation(param.index);
    }
  });
}
