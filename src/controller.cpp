#include <Arduino.h>
#include "controller.h"
#include "common.h"

SwitchMode currentMode = EMPTY;
SwitchMode lastMode = EMPTY;
char mode;

float sleepTimer = 0.000f;
static unsigned long ledOnTime = 0;

void setupController() {
  randomSeed(micros());

  // Set up switch pins with internal pull-ups
  pinMode(yButtonPin, INPUT_PULLUP);
  pinMode(gButtonPin, INPUT_PULLUP);
  // Initialize LED to a default state (yellow for MODE_NEITHER)
  updateRGBLED('X');
  digitalWrite(ledPin, HIGH);
}

void detectButtonChange() {
  // Read switch states (LOW means pressed due to pull-ups)
  bool yellowbuttonPressed = (digitalRead(yButtonPin) == LOW);
  bool greenbuttonPressed = (digitalRead(gButtonPin) == LOW);

  if (greenbuttonPressed) {
    currentMode = GREEN;
  }
  else if (yellowbuttonPressed) {
    currentMode = YELLOW;
  }

  // Update only if the mode has changed
  if (currentMode != lastMode) {
    lastMode = currentMode;
    mode = (currentMode == GREEN) ? 'G' : 'Y';
    // Update the RGB LED locally
    updateRGBLED(mode);
    sendCommand(mode);
    // Debug output
    printf("Controller mode: %c\n", mode);
    sleepTimer = 0;
  }
}

void sendCurrentMode(char mode) {
  static unsigned long lastSend = 0;
  static unsigned long interval = 500;
  unsigned long now = millis();

  if (now - lastSend >= interval) {
    lastSend = now;
    sendCommand(mode);
  }

  interval = 500 + random(-50, 51);
}

void sendCommand(char mode) {
  digitalWrite(ledPin, LOW);

  // Send the mode over RF
  char checksum = mode ^ 0xAA;
  RFSerial.write(0x7E);
  RFSerial.write(mode);
  RFSerial.write(checksum);
  RFSerial.write(0x7F);

  printf("Sent mode: %c\n", mode);
  ledOnTime = millis() + 50;
}

void loopController() {
    // --- Controller (Sender) Code ---
  detectButtonChange();
  sendCurrentMode(mode);

  if (millis() > ledOnTime) {
    digitalWrite(ledPin, HIGH);
  }

  if (sleepTimer >= 10) {
    mode = 'X';
    updateRGBLED(mode);
    lastMode = EMPTY;
    currentMode = EMPTY;
  }
  
  sleepTimer += 0.1f;
  delay(100); // Short delay for debouncing and to reduce serial output spam
}

