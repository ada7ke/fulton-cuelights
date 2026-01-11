#pragma once
#include "common.h"

#define rButtonPin 10
#define yButtonPin 3
#define gButtonPin 4

void setupController();
void loopController();
void detectButtonChange();
void sendCurrentMode(Mode mode, bool mode_r);
void sendCommand(Mode mode, bool mode_r);
void updateRGBLED(Mode mode);