#include <Arduino.h>
#include "controller.h"
#include "common.h"

Mode currentMode = Mode::X;
Mode lastMode = Mode::X;
static unsigned long ledOnTime = 0;
static unsigned long lastSend = 0;
static unsigned long interval = 1000;
static unsigned long lastActivity = 0;
static unsigned long sleepTimer = 5000;

void setupController() {
  randomSeed(micros());

  pinMode(rButtonPin, INPUT_PULLUP);
  pinMode(yButtonPin, INPUT_PULLUP);
  pinMode(gButtonPin, INPUT_PULLUP);

  updateRGBLED(currentMode);
  digitalWrite(ledPin, HIGH);
  lastActivity = millis();
}

void loopController() {
  detectButtonChange();
  sendCurrentMode(currentMode, mode_r);
  // printf("lastMode: %c, currentMode: %c\n", static_cast<char>(lastMode), static_cast<char>(currentMode));

  if (millis() > ledOnTime) {
    digitalWrite(ledPin, HIGH);
  }

  if (millis() - lastActivity >= sleepTimer && currentMode != Mode::X) {
    currentMode = Mode::X;
    lastMode = Mode::X;
    updateRGBLED(currentMode);
    sendCommand(currentMode, mode_r);
    lastActivity = millis();
  }

  while (RFSerial.available()) {
    uint8_t raw = RFSerial.read();
    if (isValidModeByte(raw)) {
      updateRGBLED(static_cast<Mode>(raw));
    }
    else {
      updateRGBLED(Mode::B);
    }
  }
  delay(100);
}

void detectButtonChange() {
  static bool lastRButton = false;

  bool rButton = (digitalRead(rButtonPin) == LOW);
  bool yButton = (digitalRead(yButtonPin) == LOW);
  bool gButton = (digitalRead(gButtonPin) == LOW);

  bool stateChanged = false;
  if (gButton && currentMode != Mode::G) {
    currentMode = Mode::G;
    stateChanged = true;
  }
  else if (yButton && currentMode != Mode::Y) {
    currentMode = Mode::Y;
    stateChanged = true;
  }
  printf("rButton: %d, lastRButton: %d\n, mode_r: %d\n", rButton, lastRButton, mode_r);
  if (lastRButton!= rButton) {
    mode_r = !mode_r;
    stateChanged = true;
  }

  lastRButton = rButton;

  if (stateChanged) {
    lastMode = currentMode;
    sendCommand(currentMode, mode_r);
    updateRGBLED(currentMode);
    printf("Controller mode: %c  Red: %d\n", toChar(currentMode), mode_r);
    lastActivity = millis();
  }
}


void sendCurrentMode(Mode mode, bool mode_r) {

  if (millis() - lastSend >= interval) {
    lastSend = millis();
    sendCommand(mode, mode_r);
    interval = 1000 + random(-50, 51);
  }
}

void sendCommand(Mode mode, bool mode_r) {
  digitalWrite(ledPin, LOW);

  uint8_t payload[2] = { 
    static_cast<uint8_t>(mode), 
    static_cast<uint8_t>(mode_r) 
  };

  uint8_t crc = crc8(payload, 2);
  const uint8_t frame[5] = { 
    FRAME_START, 
    payload[0],
    payload[1], 
    crc, 
    FRAME_END 
  };
  RFSerial.write(frame, sizeof(frame));

  printf("Sent mode: %c\n", toChar(mode));
  printf("mode_r: %d\n", mode_r ? 1 : 0);
  // printf("CRC: 0x%02X\n", crc);
  ledOnTime = millis() + 50;
}