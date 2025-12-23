#include <Arduino.h>
#include "controller.h"
#include "common.h"

Mode currentMode = Mode::X;
Mode lastMode = Mode::X;

float sleepTimer;
static unsigned long ledOnTime = 0;
static unsigned long lastSend = 0;
static unsigned long interval = 1000;

void setupController() {
  randomSeed(micros());

  pinMode(rButtonPin, INPUT_PULLUP);
  pinMode(yButtonPin, INPUT_PULLUP);
  pinMode(gButtonPin, INPUT_PULLUP);

  updateRGBLED(currentMode);
  digitalWrite(ledPin, HIGH);

  sleepTimer = 0.0f;
}

void loopController() {
  detectButtonChange();
  sendCurrentMode(currentMode);
  printf("lastMode: %c, currentMode: %c\n", static_cast<char>(lastMode), static_cast<char>(currentMode));
  printf("sleepTimer: %.3f\n", sleepTimer);

  if (millis() > ledOnTime) {
    digitalWrite(ledPin, HIGH);
  }

  if (sleepTimer >= 15.0f) {
    currentMode = Mode::X;
    updateRGBLED(currentMode);
    lastMode = Mode::X;
    currentMode = Mode::X;
  }

  while (RFSerial.available()) {
    uint8_t raw = RFSerial.read();
    switch(raw) {
      case 'X':
      case 'R':
      case 'Y':
      case 'G':
        updateRGBLED(static_cast<Mode>(raw));
        break;
      default:
        break;
    }
  }

  sleepTimer += 0.1f;
  delay(100);
}

void detectButtonChange() {
  bool rButtonPressed = (digitalRead(rButtonPin) == LOW);
  bool yButtonPressed = (digitalRead(yButtonPin) == LOW);
  bool gButtonPressed = (digitalRead(gButtonPin) == LOW);

  if (rButtonPressed) {
    currentMode = Mode::R;
  }
  else if (gButtonPressed) {
    currentMode = Mode::G;
  }
  else if (yButtonPressed) {
    currentMode = Mode::Y;
  }

  if (currentMode != lastMode) {
    lastMode = currentMode;
    sendCommand(currentMode);
    printf("Controller mode: %c\n", static_cast<char>(currentMode));
    sleepTimer = 0.0f;
  }
}

void sendCurrentMode(Mode mode) {
  unsigned long now = millis();

  if (now - lastSend >= interval) {
    lastSend = now;
    sendCommand(mode);
    interval = 1000 + random(-50, 51);
  }
}

void sendCommand(Mode mode) {
  digitalWrite(ledPin, LOW);

  uint8_t payload[1] = { static_cast<uint8_t>(mode) };
  uint8_t crc = crc8(payload, 1);

  RFSerial.write(0x7E);
  RFSerial.write(payload[0]);
  RFSerial.write(crc);
  RFSerial.write(0x7F);

  printf("Sent mode: %c\n", static_cast<char>(mode));
  ledOnTime = millis() + 50;
}