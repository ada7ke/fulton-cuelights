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

  pinMode(rButtonPin, INPUT_PULLUP);
  pinMode(yButtonPin, INPUT_PULLUP);
  pinMode(gButtonPin, INPUT_PULLUP);

  updateRGBLED('X');
  digitalWrite(ledPin, HIGH);
}

void loopController() {
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

  while (RFSerial.available()) {
    mode = RFSerial.read();
    updateRGBLED(mode);
  }

  sleepTimer += 0.1f;
  delay(100);
}

void detectButtonChange() {
  bool rButtonPressed = (digitalRead(rButtonPin) == LOW);
  bool yButtonPressed = (digitalRead(yButtonPin) == LOW);
  bool gButtonPressed = (digitalRead(gButtonPin) == LOW);

  if (rButtonPressed) {
    currentMode = RED;
  }
  else if (gButtonPressed) {
    currentMode = GREEN;
  }
  else if (yButtonPressed) {
    currentMode = YELLOW;
  }

  if (currentMode != lastMode) {
    lastMode = currentMode;
    mode = (currentMode == GREEN) ? 'G' : 'Y';
    sendCommand(mode);
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

  uint8_t buffer[1] = { static_cast<uint8_t>(mode) };
  uint8_t crc = crc8(buffer, 1);

  RFSerial.write(0x7E);
  RFSerial.write(mode);
  RFSerial.write(crc);
  RFSerial.write(0x7F);

  printf("Sent mode: %c\n", mode);
  ledOnTime = millis() + 50;
}