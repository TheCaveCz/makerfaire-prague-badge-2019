//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Ring animations basic functions
//

NeoGamma<NeoGammaTableMethod> ringGamma;
NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod> ringLeds(RING_COUNT, 0);
NeoPixelAnimator ringAnimations(4);


RgbColor ringRed(150, 0, 0);
RgbColor ringGreen(0, 150, 0);
RgbColor ringBlue(0, 0, 150);
RgbColor ringWhite(150, 150, 150);
RgbColor ringBlack(0, 0, 0);

typedef enum {
  RA_NONE,
  RA_RED,
  RA_GREEN,
  RA_BLUE,
  RA_RG,
  RA_RB,
  RA_GB,
  RA_RGB,
} RingAnimState;

RingAnimState ringAnimState;

RgbColor ringChannels[2][RING_COUNT];
uint8_t ringNextChannel;


void ringSetAnimState(RingAnimState newState);


void ringSetAnimState(RingAnimState newState) {
  if (newState == ringAnimState) return;
  ringAnimState = newState;

  switch (ringAnimState) {
    case RA_RED:
      ringPulseAnim(ringNextChannel, ringRed);
      break;
    case RA_GREEN:
      ringPulseAnim(ringNextChannel, ringGreen);
      break;
    case RA_BLUE:
      ringPulseAnim(ringNextChannel, ringBlue);
      break;
    case RA_RGB:
      ringAllContestedAnim(ringNextChannel);
      break;
    case RA_RG:
      ringTwoContestedAnim(ringNextChannel, ringRed, ringGreen);
      break;
    case RA_RB:
      ringTwoContestedAnim(ringNextChannel, ringRed, ringBlue);
      break;
    case RA_GB:
      ringTwoContestedAnim(ringNextChannel, ringGreen, ringBlue);
      break;
    default: break;
  }
  uint8_t ch = ringNextChannel ? 0 : 1;
  ringRunBlender(ch, ringNextChannel);
  ringNextChannel = ch;
}

void ringRunBlender(const uint8_t oldChannel, const uint8_t newChannel) {
  ringAnimations.StartAnimation(2, 5000, [ = ](const AnimationParam & param) {
    for (uint8_t i = 0; i < RING_COUNT; i++) {
      ringLeds.SetPixelColor(i, ringGamma.Correct(RgbColor::LinearBlend(ringChannels[oldChannel][i], ringChannels[newChannel][i], NeoEase::CubicInOut(param.progress))));
    }

    if (param.state == AnimationState_Completed)  {
      ringKeepBlender(newChannel);
    }
  });
}

void ringKeepBlender(const uint8_t ch) {
  ringAnimations.StartAnimation(2, 5000, [ = ](const AnimationParam & param) {
    for (uint8_t i = 0; i < RING_COUNT; i++) {
      ringLeds.SetPixelColor(i, ringGamma.Correct(ringChannels[ch][i]));
    }
    if (param.state == AnimationState_Completed)  {
      ringAnimations.RestartAnimation(param.index);
    }
  });
}

void ringSetup() {
  ringLeds.Begin();
  ringLeds.ClearTo(ringBlack);
  ringNextChannel = 0;
  ringSet(0, ringBlack);
  ringSet(1, ringBlack);
  ringAnimState = RA_NONE;
}
