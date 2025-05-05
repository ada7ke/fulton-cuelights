#pragma once

#define yellowbuttonPin 3  // Connect left switch here
#define greenbuttonPin 4 // Connect right switch here

enum SwitchMode
{
  MODE_RED,
  MODE_YELLOW,
  MODE_GREEN
};

void setupController();
void loopController();