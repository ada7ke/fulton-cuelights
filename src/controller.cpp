#include <Arduino.h>
#include "controller.h"
#include "common.h"

int systemMode = 1;
static Mode currentMode = Mode::X;
bool stateChanged = false;

static unsigned long lastSend = 0;
static unsigned long sendInterval = 500;
static unsigned long lastActivity = 0;
static unsigned long sleepTimer = 5000;

struct ButtonState {
  bool r = false;
  bool y = false;
  bool g = false;
  bool rPressed = false;
  bool yPressed = false;
  bool gPressed = false;
  bool yLong = false;
  bool gLong = false;
};

static ButtonState btn;

static const uint8_t brightnessOptions[] = { 1, 5, 10, 15 };
static uint8_t brightnessIndex = 1; // default to 5
static uint8_t mode2Mask = 0; // bit0=yellow, bit1=green

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

  if (btn.gLong) {
    btn.gLong = false;
    systemMode = (systemMode == 1) ? 2 : 1;
    printf("System mode changed to: %d\n", systemMode);
    ledBlink(systemMode); // blue blink 1 or 2 times
    lastActivity = millis();
  }
  if (btn.yLong) {
    btn.yLong = false;
    brightnessIndex = (brightnessIndex + 1) % (sizeof(brightnessOptions) / sizeof(brightnessOptions[0]));
    printf("Brightness level changed to: %u\n", brightnessOptions[brightnessIndex]);
    ledBlink(brightnessIndex + 1);
    stateChanged = true;
  }

  if (systemMode == 1) {
    mode1();
  } else {
    mode2();
  }

  while (RFSerial.available()) {
    Frame f;
    if (readFrame(f) && f.device == RECEIVER_ADDRESS) {
      printf("Receiver echo R:%u Y:%u G:%u Bright:%u\n", f.red, f.yellow, f.green, f.brightness);
    }
  }

  delay(100);
}

void mode1() {
  stateChanged = false;

  if (btn.g && currentMode != Mode::G) {
    currentMode = Mode::G;
    stateChanged = true;
  } else if (btn.y && currentMode != Mode::Y) {
    currentMode = Mode::Y;
    stateChanged = true;
  }

  if (btn.rPressed) {
    btn.rPressed = false;
    mode_r = !mode_r;
    stateChanged = true;
  }

  // auto-sleep after inactivity
  if (millis() - lastActivity >= sleepTimer && currentMode != Mode::X) {
    currentMode = Mode::X;
    lastActivity = millis();
  }

  uint8_t red = mode_r ? 1 : 0;
  uint8_t yellow = (currentMode == Mode::Y) ? 1 : 0;
  uint8_t green = (currentMode == Mode::G) ? 1 : 0;

  sendCurrentState(red, yellow, green);
  if (stateChanged) {
    sendCommand(red, yellow, green);
    printf("Controller mode: %c  R:%u Y:%u G:%u\n", toChar(currentMode), red, yellow, green);
    lastActivity = millis();
  }

  updateRGBLED(currentMode);
}

void mode2() {
  stateChanged = false;

  if (btn.yPressed) {
    btn.yPressed = false;
    mode2Mask ^= 0x01;
    stateChanged = true;
  }
  if (btn.gPressed) {
    btn.gPressed = false;
    mode2Mask ^= 0x02;
    stateChanged = true;
  }

  // Red is a latching button: use the current physical state.
  static bool lastRedLatch = false;
  if (btn.r != lastRedLatch) {
    lastRedLatch = btn.r;
    stateChanged = true;
  }

  uint8_t red = btn.r ? 1 : 0;
  uint8_t yellow = (mode2Mask & 0x01) ? 1 : 0;
  uint8_t green = (mode2Mask & 0x02) ? 1 : 0;

  sendCurrentState(red, yellow, green);
  if (stateChanged) {
    sendCommand(red, yellow, green);
    printf("Mode2 state: R:%u Y:%u G:%u\n", red, yellow, green);
    lastActivity = millis();
  }

  if (green && yellow) {
    updateRGBLED(Mode::T);
  } else if (green) {
    updateRGBLED(Mode::G);
  } else if (yellow) {
    updateRGBLED(Mode::R);
  } else {
    updateRGBLED(Mode::X);
  }  
}

void ledBlink(int times) {
  for (int i = 0; i < times; ++i) {
    updateRGBLED(Mode::B);
    delay(200);
    updateRGBLED(Mode::X);
    delay(150);
  }
}

void detectButtonChange() {
  static bool lastR = false;
  static bool lastY = false;
  static bool lastG = false;

  static unsigned long yPressStart = 0;
  static bool yLongHandled = false;

  static unsigned long gPressStart = 0;
  static bool gLongHandled = false;

  // single events
  btn.rPressed = false;
  btn.yPressed = false;
  btn.gPressed = false;
  btn.yLong = false;
  btn.gLong = false;

  btn.r = (digitalRead(rButtonPin) == LOW);
  btn.y = (digitalRead(yButtonPin) == LOW);
  btn.g = (digitalRead(gButtonPin) == LOW);

  if (btn.r && !lastR) btn.rPressed = true;
  if (btn.y && !lastY) btn.yPressed = true;
  if (btn.g && !lastG) btn.gPressed = true;

  // yellow long hold
  if (btn.y && !lastY) {
    yPressStart = millis();
    yLongHandled = false;
  }
  if (!btn.y && lastY) {
    yPressStart = 0;
    yLongHandled = false;
  }
  if (btn.y && yPressStart != 0 && !yLongHandled && (millis() - yPressStart >= longHoldDuration)) {
    btn.yLong = true;
    yLongHandled = true;
  }

  // green long hold 
  if (btn.g && !lastG) {
    gPressStart = millis();
    gLongHandled = false;
  }
  if (!btn.g && lastG) {
    gPressStart = 0;
    gLongHandled = false;
  }
  if (btn.g && gPressStart != 0 && !gLongHandled && (millis() - gPressStart >= longHoldDuration)) {
    btn.gLong = true;
    gLongHandled = true;
  }

  lastR = btn.r;
  lastY = btn.y;
  lastG = btn.g;
}

void sendCurrentState(uint8_t red, uint8_t yellow, uint8_t green) {
  if (millis() - lastSend >= sendInterval) {
    lastSend = millis();
    sendCommand(red, yellow, green);
    sendInterval = 500 + random(-50, 51);
  }
}

void sendCommand(uint8_t red, uint8_t yellow, uint8_t green) {
  digitalWrite(LED_PIN, LOW);
  Frame frame = { CONTROLLER_ADDRESS, red, yellow, green, brightnessOptions[brightnessIndex] };
  sendFrame(frame);
  printf("Sent state R:%u Y:%u G:%u\n", red, yellow, green);
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
    case Mode::T: // yellow and green
      analogWrite(rPin, 255);
      analogWrite(gPin, 200);
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