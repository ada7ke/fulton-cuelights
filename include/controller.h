#pragma once
#include "common.h"

constexpr uint8_t rButtonPin = 10;
constexpr uint8_t yButtonPin = 3;
constexpr uint8_t gButtonPin = 4;

void setupController();
void loopController();

// Helpers (also used internally by controller.cpp)
void cycleBrightness();
void cycleSystemMode();
void handleControllerShortcuts();
void runSystemMode();
void processReceiverEcho();

bool handleMode2TogglePress(bool &pressedFlag, unsigned long &lockoutUntil, uint8_t bitMask);
void updateEdgeAndOptionalLongHold(
	bool now,
	bool &last,
	bool &pressedEvent,
	unsigned long *pressStart,
	bool *longHandled,
	bool *longEvent
);

void setRgb(uint8_t red, uint8_t green, uint8_t blue);
void mode1();
void mode2();
void mode3();
void ledBlink(int times);
void detectButtonChange();
void sendCurrentState(uint8_t red, uint8_t yellow, uint8_t green);
void sendCommand(uint8_t red, uint8_t yellow, uint8_t green);
void updateRGBLED(uint8_t red, uint8_t yellow, uint8_t green, uint8_t blue);