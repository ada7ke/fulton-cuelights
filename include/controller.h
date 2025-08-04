#pragma once

#define rButtonPin 6
#define yButtonPin 3
#define gButtonPin 4

enum SwitchMode
{
  EMPTY,
  R,
  Y,
  G,
};

void setupController();
void loopController();
void detectButtonChange();
void sendCurrentMode(char mode);
void sendCommand(char mode);