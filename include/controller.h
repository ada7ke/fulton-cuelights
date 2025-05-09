#pragma once

#define yButtonPin 3  // Connect left switch here
#define gButtonPin 4 // Connect right switch here

enum SwitchMode
{
  EMPTY,
  YELLOW,
  GREEN,
};

void setupController();
void loopController();
void detectButtonChange();
void sendCurrentMode(char mode);
void sendCommand(char mode);