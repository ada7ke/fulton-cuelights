#pragma once
#include "common.h"

#define rButtonPin 10
#define yButtonPin 3
#define gButtonPin 4

void setupController();
void loopController();
void mode1();
void mode2();
void ledBlink(int times);
void detectButtonChange();
void sendCurrentState(uint8_t red, uint8_t yellow, uint8_t green);
void sendCommand(uint8_t red, uint8_t yellow, uint8_t green);
void updateRGBLED(uint8_t red, uint8_t yellow, uint8_t green, uint8_t blue);