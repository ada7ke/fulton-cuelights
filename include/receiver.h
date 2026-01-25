#pragma once
#include "common.h"

#define redLED 6
#define yellowLED 7
#define greenLED 9

void setupReceiver();
void loopReceiver();

void processCommand();
void sendIntervaledEcho();
void timeoutReceiver();
void updateLEDs(uint8_t red, uint8_t yellow, uint8_t green);