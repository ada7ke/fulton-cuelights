#include <Arduino.h>
#include "controller.h"
#include "common.h"

// setup modes
int systemMode = 1;
static Mode currentMode = Mode::X;
bool stateChanged = false;

// setup rf vars
static unsigned long lastSend = 0;
static unsigned long sendInterval = 500;
static unsigned long lastActivity = 0;
static unsigned long sleepTimer = 5000;

// setup button detections
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

static constexpr unsigned long longHoldDuration = 3000UL;
static const uint8_t brightnessOptions[] = { 5, 15, 30 };
static uint8_t brightnessIndex = 1; // default to 15
static uint8_t mode2Mask = 0; // bit0=yellow, bit1=green

static constexpr size_t brightnessOptionsCount = sizeof(brightnessOptions) / sizeof(brightnessOptions[0]);
static constexpr unsigned long mode2ToggleDebounceMs = 200;

static inline uint8_t boolToByte(bool v) { return v ? 1 : 0; }

void setupController() {
  randomSeed(micros());

  pinMode(rButtonPin, INPUT_PULLUP);
  pinMode(yButtonPin, INPUT_PULLUP);
  pinMode(gButtonPin, INPUT_PULLUP);

  updateRGBLED(0, 0, 0, 0);
  digitalWrite(LED_PIN, HIGH);
  lastActivity = millis();
}

void loopController() {
  serviceActivityLed();
  detectButtonChange();

  handleControllerShortcuts();
  runSystemMode();
  processEcho();
}

// blink blue led on long hold button events
void ledBlink(int times) {
  for (int i = 0; i < times; ++i) {
    updateRGBLED(0, 0, 0, 1);
    delay(200);
    updateRGBLED(0, 0, 0, 0);
    delay(150);
  }
}

// cycle between led brightness levels
void cycleBrightness() {
  brightnessIndex = (brightnessIndex + 1) % brightnessOptionsCount;
  printf("Brightness level changed to: %u\n", brightnessOptions[brightnessIndex]);
  ledBlink(static_cast<int>(brightnessIndex) + 1);
  stateChanged = true;
}

// cycle between system modes 1, 2, and 3
void cycleSystemMode() {
  systemMode = (systemMode % 3) + 1;
  printf("System mode changed to: %d\n", systemMode);
  ledBlink(systemMode);
  lastActivity = millis();
}

// handle long hold actions
void handleControllerShortcuts() {
  if (btn.gLong) {
    btn.gLong = false;
    cycleSystemMode();
  }
  if (btn.yLong) {
    btn.yLong = false;
    cycleBrightness();
  }
}

void runSystemMode() {
  switch (systemMode) {
    case 1: mode1(); break;
    case 2: mode2(); break;
    default: mode3(); break;
  }
}

// display receiver echo states on controller rgb led
void processEcho() {
  while (RFSerial.available()) {
    Frame f;
    if (readFrame(f) && f.device == RECEIVER_ADDRESS) {
      updateRGBLED(f.red, f.yellow, f.green, 0);
      // printf("Receiver echo R:%u Y:%u G:%u Brightness:%u\n", f.red, f.yellow, f.green, f.brightness);
    }
  }
}

// mode 2 toggle with debounce
bool mode2Toggle(bool &pressedFlag, unsigned long &lockoutUntil, uint8_t bitMask) {
  if (!pressedFlag) return false;
  pressedFlag = false;

  if ((long)(millis() - lockoutUntil) < 0) return false;
  mode2Mask ^= bitMask;
  lockoutUntil = millis() + mode2ToggleDebounceMs;
  return true;
}

// update button events and long-press detection
void updateButtonEvents(
  bool now,
  bool &last,
  bool &pressedEvent,
  unsigned long *pressStart,
  bool *longHandled,
  bool *longEvent
) {
  pressedEvent = (now && !last);

  if (pressStart && longHandled && longEvent) {
    if (now && !last) {
      *pressStart = millis();
      *longHandled = false;
    }

    if (!now && last) {
      *pressStart = 0;
      *longHandled = false;
    }

    if (now && *pressStart != 0 && !*longHandled && (millis() - *pressStart >= longHoldDuration)) {
      *longEvent = true;
      *longHandled = true;
    }
  }

  last = now;
}

// mode 1: swap between yellow and green modes. red runs independently
void mode1() {
  stateChanged = false;

  // handle button presses
  if (btn.gPressed) {
    btn.gPressed = false;
    if (currentMode != Mode::G) {
      currentMode = Mode::G;
      stateChanged = true;
    }
  } else if (btn.yPressed) {
    btn.yPressed = false;
    if (currentMode != Mode::Y) {
      currentMode = Mode::Y;
      stateChanged = true;
    }
  }

  if (btn.rPressed) {
    btn.rPressed = false;
    mode_r = !mode_r;
    stateChanged = true;
  }

  // auto-sleep after 5 seconds inactivity
  if (millis() - lastActivity >= sleepTimer && currentMode != Mode::X) {
    currentMode = Mode::X;
    lastActivity = millis();
  }

  // send led state if changed
  const uint8_t red = boolToByte(mode_r);
  const uint8_t yellow = boolToByte(currentMode == Mode::Y);
  const uint8_t green = boolToByte(currentMode == Mode::G);

  sendCurrentState(red, yellow, green);
  if (stateChanged) {
    printf("Mode1 state changed to R:%u Y:%u G:%u ", red, yellow, green);
    sendCommand(red, yellow, green);
    lastActivity = millis();
  }
}

// mode 2: all colors toggle independently
void mode2() {
  stateChanged = false;

  static unsigned long yToggleLockoutUntil = 0;
  static unsigned long gToggleLockoutUntil = 0;

  stateChanged |= mode2Toggle(btn.yPressed, yToggleLockoutUntil, 0x01);
  stateChanged |= mode2Toggle(btn.gPressed, gToggleLockoutUntil, 0x02);

  const uint8_t red = boolToByte(btn.r);
  const uint8_t yellow = boolToByte(mode2Mask & 0x01);
  const uint8_t green = boolToByte(mode2Mask & 0x02);

  // send led state if changed
  sendCurrentState(red, yellow, green);
  if (stateChanged) {
    sendCommand(red, yellow, green);
    lastActivity = millis();
  }
}

// mode 3: leds on while button is held
void mode3() {
  static bool lastR = false;
  static bool lastY = false;
  static bool lastG = false;

  const bool rNow = btn.r;
  const bool yNow = btn.y;
  const bool gNow = btn.g;

  const uint8_t red = boolToByte(rNow);
  const uint8_t yellow = boolToByte(yNow);
  const uint8_t green = boolToByte(gNow);

  stateChanged = (rNow != lastR) || (yNow != lastY) || (gNow != lastG);

  // send led state if changed
  sendCurrentState(red, yellow, green);
  if (stateChanged) {
    sendCommand(red, yellow, green);
    lastActivity = millis();
    lastR = rNow;
    lastY = yNow;
    lastG = gNow;
  }
}

// handle button state changes and long-press detection
void detectButtonChange() {
  static bool lastR = false;
  static bool lastY = false;
  static bool lastG = false;

  static unsigned long yPressStart = 0;
  static bool yLongHandled = false;

  static unsigned long gPressStart = 0;
  static bool gLongHandled = false;

  btn.rPressed = false;
  btn.yPressed = false;
  btn.gPressed = false;
  btn.yLong = false;
  btn.gLong = false;

  btn.r = (digitalRead(rButtonPin) == LOW);
  btn.y = (digitalRead(yButtonPin) == LOW);
  btn.g = (digitalRead(gButtonPin) == LOW);
  // printf("Buttons state R:%d Y:%d G:%d\n", btn.r, btn.y, btn.g);m

  updateButtonEvents(btn.r, lastR, btn.rPressed, nullptr, nullptr, nullptr);
  updateButtonEvents(btn.y, lastY, btn.yPressed, &yPressStart, &yLongHandled, &btn.yLong);
  updateButtonEvents(btn.g, lastG, btn.gPressed, &gPressStart, &gLongHandled, &btn.gLong);
}

// send current led state at intervals
void sendCurrentState(uint8_t red, uint8_t yellow, uint8_t green) {
  if (millis() - lastSend >= sendInterval) {
    lastSend = millis();
    sendCommand(red, yellow, green);
    sendInterval = 500 + random(-50, 51);
  }
}

// send command frame to receiver
void sendCommand(uint8_t red, uint8_t yellow, uint8_t green) {
  pulseActivityLed(10);
  Frame frame = { CONTROLLER_ADDRESS, red, yellow, green, brightnessOptions[brightnessIndex] };
  sendFrame(frame);
  // printf("Sent state R:%u Y:%u G:%u\n", red, yellow, green);
}

// set rgb led color
void setRgb(uint8_t red, uint8_t green, uint8_t blue) {
  analogWrite(rPin, red);
  analogWrite(gPin, green);
  analogWrite(bPin, blue);
}

// update rgb led based on receiver state
void updateRGBLED(uint8_t red, uint8_t yellow, uint8_t green, uint8_t blue) {
  if (blue) {
    setRgb(0, 0, 255);
  } else if (yellow && green) {
    setRgb(255, 200, 0);
  } else if (green) {
    setRgb(0, 255, 0);
  } else if (yellow) {
    if (systemMode == 1) {
      setRgb(255, 100, 0);
    } else {
      // systemMode 2/3: map yellow to red
      setRgb(255, 0, 0);
    }
  } else if (red) {
    setRgb(255, 0, 255);
  } else {
    setRgb(0, 0, 0);
  }
}
