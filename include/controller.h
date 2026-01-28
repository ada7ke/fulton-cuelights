#pragma once
#include "common.h"

constexpr uint8_t rButtonPin = 10;
constexpr uint8_t bButtonPin = 3;
constexpr uint8_t gButtonPin = 4;

void setupController();
void loopController();

void cycleBrightness();
void cycleSystemMode();
void handleControllerShortcuts();
void runSystemMode();
void processEcho();
void errorBlink();
void modeBlink(int times);

bool toggleColorBit(bool &pressedFlag, unsigned long &lockoutUntil, uint8_t bitMask);
void updateButtonEvents(bool now, bool &last, 
						bool &pressedEvent, unsigned long *pressStart, 
						bool *longHandled, bool *longEvent);

void mode1();
void mode2();
void mode3();
void detectButtonChange();
void sendCurrentState();
void sendCommand(uint8_t red, uint8_t green, uint8_t blue);
void updateRGBLED(uint8_t red, uint8_t green, uint8_t blue);