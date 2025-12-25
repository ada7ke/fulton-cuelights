#pragma once
#include "common.h"

#define redLED 6
#define yellowLED 7
#define greenLED 9

enum State
{
  WAIT_START,
  READ_MODE,
  READ_RED,
  READ_CHECKSUM,
  WAIT_END
};

void setupReceiver();
void loopReceiver();
void updateLEDs(Mode mode, bool mode_r);