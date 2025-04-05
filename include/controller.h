#pragma once

#define leftSwitchPin 3  // Connect left switch here
#define rightSwitchPin 4 // Connect right switch here

enum SwitchMode
{
  MODE_LEFT,
  MODE_NEITHER,
  MODE_RIGHT
};

void setupController();
void loopController();