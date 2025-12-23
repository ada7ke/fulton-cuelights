#pragma once

#define rButtonPin 6
#define yButtonPin 3
#define gButtonPin 4

void setupController();
void loopController();
void detectButtonChange();
void sendCurrentMode(Mode mode);
void sendCommand(Mode mode);