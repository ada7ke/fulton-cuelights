#pragma once
#include "common.h"

#define redLED 6
#define yellowLED 7
#define greenLED 9

void setupReceiver();
void loopReceiver();
void updateLEDs(Mode mode, bool mode_r);