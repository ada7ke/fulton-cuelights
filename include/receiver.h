#pragma once
#include "common.h"

#define redLED 6
#define greenLED 9
#define blueLED 7

void setupReceiver();
void loopReceiver();

void processCommand();
void sendIntervaledEcho();
void timeoutReceiver();
void updateLEDs(uint8_t red, uint8_t green, uint8_t blue);