#include <Arduino.h>
#include "controller.h"
#include "common.h"

Mode currentMode = Mode::X;
Mode lastMode = Mode::X;
Mode RGBLEDMode = currentMode;
static unsigned long lastSend = 0;
static unsigned long interval = 1000;
static unsigned long lastActivity = 0;
static unsigned long sleepTimer = 5000;

static const uint8_t brightnessOptions[] = { 1, 5, 10, 15 };
static uint8_t brightnessIndex = 1; // default to 5

void setupController() {
  randomSeed(micros());

  pinMode(rButtonPin, INPUT_PULLUP);
  pinMode(yButtonPin, INPUT_PULLUP);
  pinMode(gButtonPin, INPUT_PULLUP);

  updateRGBLED(currentMode);
  digitalWrite(LED_PIN, HIGH);
  lastActivity = millis();
}

void loopController() {
  detectButtonChange();
  sendCurrentMode(currentMode, mode_r);

  if (millis() - lastActivity >= sleepTimer && currentMode != Mode::X) {
    currentMode = Mode::X;
    lastMode = Mode::X;
    RGBLEDMode = currentMode;
    sendCommand(currentMode, mode_r);
    lastActivity = millis();
  }

  while (RFSerial.available()) {
    Frame f;
    if (readFrame(f)) {
      if (f.device == RECEIVER_ADDRESS) {
        RGBLEDMode = static_cast<Mode>(f.mode);
      }
    }
  }

  updateRGBLED(RGBLEDMode);
  delay(100);
}

void detectButtonChange() {
  static bool lastRButton = false;
  static bool lastYButton = false;
  static unsigned long yPressStart = 0;
  static bool yLongHandled = false;

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

  if (yButton && !lastYButton) {
    yPressStart = millis();
    yLongHandled = false;
  }
  if (!yButton && lastYButton) {
    yPressStart = 0;
    yLongHandled = false;
  }
  if (yButton && yPressStart != 0 && !yLongHandled && (millis() - yPressStart >= longHoldDuration)) {
    brightnessIndex = (brightnessIndex + 1) % (sizeof(brightnessOptions) / sizeof(brightnessOptions[0]));
    yLongHandled = true;
    sendCommand(currentMode, mode_r);
    printf("Brightness level changed to: %u\n", brightnessOptions[brightnessIndex]);
    lastActivity = millis();
  }
  if (lastRButton!= rButton) {
    mode_r = !mode_r;
    stateChanged = true;
  }

  lastRButton = rButton;
  lastYButton = yButton;

  if (stateChanged) {
    lastMode = currentMode;
    RGBLEDMode = currentMode;
    sendCommand(currentMode, mode_r);
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
  digitalWrite(LED_PIN, LOW);
  Frame frame = { CONTROLLER_ADDRESS, static_cast<uint8_t>(mode), static_cast<uint8_t>(mode_r), brightnessOptions[brightnessIndex] };
  sendFrame(frame);
  printf("Sent mode: %c\n", toChar(mode));
  delay(10);
  digitalWrite(LED_PIN, HIGH);
}

void updateRGBLED(Mode mode)
{  
  switch (mode)
  {
    case Mode::X: // sleep
      analogWrite(rPin, 0);
      analogWrite(gPin, 0);
      analogWrite(bPin, 0);
      break;
    case Mode::G: // green
      analogWrite(rPin, 0);
      analogWrite(gPin, 255);
      analogWrite(bPin, 0);
      break;
    case Mode::Y: // yellow
      analogWrite(rPin, 255);
      analogWrite(gPin, 100);
      analogWrite(bPin, 0);
      break;
    case Mode::B: // no message timeout
      analogWrite(rPin, 0);
      analogWrite(gPin, 0);
      analogWrite(bPin, 255);
      break;
    case Mode::R: // failed checksum
      analogWrite(rPin, 255);
      analogWrite(gPin, 0);
      analogWrite(bPin, 0);
      break;
    default: // unknown error
      printf("Unknown RGB LED mode: %c\n", toChar(mode));
      analogWrite(rPin, 255);
      analogWrite(gPin, 255);
      analogWrite(bPin, 255);
      break;
  }
}