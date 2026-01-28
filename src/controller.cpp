#include <Arduino.h>
#include "controller.h"
#include "common.h"

// init
int systemMode = 1;

static unsigned long lastSend = 0;
static unsigned long lastActivity = 0;
static unsigned long sleepTimer = 5000;

struct ButtonState {
  bool r = false;
  bool b = false;
  bool g = false;
  bool rPressed = false;
  bool bPressed = false;
  bool gPressed = false;
  bool bLong = false;
  bool gLong = false;
  bool rLong = false;
};
static ButtonState btn;

bool stateChanged = false;
static constexpr unsigned long longHoldDuration = 3000UL;
static const uint8_t brightnessOptions[] = { 5, 15, 30 };
static uint8_t brightnessIndex = 1;
static constexpr size_t brightnessOptionsCount = sizeof(brightnessOptions) / sizeof(brightnessOptions[0]);
static constexpr unsigned long mode2ToggleDebounceMs = 200;

static uint8_t colorStateMask = 0;
static constexpr uint8_t BLUE_BIT = 0x01;
static constexpr uint8_t GREEN_BIT = 0x02;
static constexpr uint8_t RED_BIT = 0x04;

static bool blinking = false;
static unsigned int cyclesElapsed = 0;

static inline uint8_t boolToByte(bool v) { return v ? 1 : 0; }

static inline void getColorStates(uint8_t &red, uint8_t &blue, uint8_t &green) {
  red = boolToByte(colorStateMask & RED_BIT);
  blue = boolToByte(colorStateMask & BLUE_BIT);
  green = boolToByte(colorStateMask & GREEN_BIT);
}

void setupController() {
  randomSeed(micros());

  pinMode(rButtonPin, INPUT_PULLUP);
  pinMode(bButtonPin, INPUT_PULLUP);
  pinMode(gButtonPin, INPUT_PULLUP);

  updateRGBLED(0, 0, 0);
  digitalWrite(LED_PIN, HIGH);
  lastActivity = millis();
}

void loopController() {
  serviceActivityLed();
  detectButtonChange();

  handleControllerShortcuts();
  runSystemMode();
  processEcho();
  errorBlink();
}

// toggle color bit with debounce
bool toggleColorBit(bool &pressedFlag, unsigned long &lockoutUntil, uint8_t bitMask) {
  // ignore if button not pressed
  if (!pressedFlag) return false;
  pressedFlag = false;  
  // ignore if within debounce lockout period
  if ((long)(millis() - lockoutUntil) < 0) return false;

  // toggle the corresponding bit in colorStateMask
  colorStateMask ^= bitMask;

  lockoutUntil = millis() + mode2ToggleDebounceMs;
  return true;
}

// update button events and long-press detection
void updateButtonEvents(bool now, bool &last, 
                        bool &pressedEvent, unsigned long *pressStart, 
                        bool *longHandled, bool *longEvent) {
  pressedEvent = (now && !last);

  // long-press detection
  if (pressStart && longHandled && longEvent) {
    // on button press: record start time and reset long press flag
    if (now && !last) {
      *pressStart = millis();
      *longHandled = false;
    }
    // on button release: clear start time and reset long press flag
    if (!now && last) {
      *pressStart = 0;
      *longHandled = false;
    }
    // check for long press
    if (now && *pressStart != 0 && !*longHandled && (millis() - *pressStart >= longHoldDuration)) {
      *longEvent = true;
      *longHandled = true;
    }
  }

  last = now;
}

// handle button state changes and long-press detection
void detectButtonChange() {
  static bool lastR = false;
  static bool lastB = false;
  static bool lastG = false;

  static unsigned long rPressStart = 0;
  static bool rLongHandled = false;

  static unsigned long gPressStart = 0;
  static bool gLongHandled = false;

  static unsigned long bPressStart = 0;
  static bool bLongHandled = false;

  btn.rPressed = false;
  btn.gPressed = false;
  btn.bPressed = false;
  btn.rLong = false;
  btn.gLong = false;
  btn.bLong = false;

  btn.r = (digitalRead(rButtonPin) == LOW);
  btn.g = (digitalRead(gButtonPin) == LOW);
  btn.b = (digitalRead(bButtonPin) == LOW);
  
  // printf("Buttons state R:%d B:%d G:%d\n", btn.r, btn.b, btn.g);

  updateButtonEvents(btn.r, lastR, btn.rPressed, &rPressStart, &rLongHandled, &btn.rLong);
  updateButtonEvents(btn.g, lastG, btn.gPressed, &gPressStart, &gLongHandled, &btn.gLong);
  updateButtonEvents(btn.b, lastB, btn.bPressed, &bPressStart, &bLongHandled, &btn.bLong);
}

// handle long hold actions
void handleControllerShortcuts() {
  // hold red + green to change system mode
  if (btn.rLong && btn.gLong) {
    btn.rLong = false;
    btn.gLong = false;
    cycleSystemMode();
  }

  // hold red + blue to change led brightness
  if (btn.rLong && btn.bLong) {
    btn.rLong = false;
    btn.bLong = false;
    cycleBrightness();
  }
}

// blink yellow rgb led on long hold button events
void modeBlink(int times) {
  for (int i = 0; i < times; ++i) {
    blinking = true;
    updateRGBLED(1, 1, 0);
    delay(200);
    updateRGBLED(0, 0, 0);
    delay(150);
  }
  blinking = false;
}

// cycle between led brightness levels
void cycleBrightness() {
  brightnessIndex = (brightnessIndex + 1) % brightnessOptionsCount;
  printf("Brightness level changed to: %u\n", brightnessOptions[brightnessIndex]);
  modeBlink(static_cast<int>(brightnessIndex) + 1);
  stateChanged = true;
  lastActivity = millis();
}

// cycle between system modes 1, 2, and 3
void cycleSystemMode() {
  systemMode = (systemMode % 3) + 1;
  printf("System mode changed to: %d\n", systemMode);
  modeBlink(systemMode);
  lastActivity = millis();
}

void runSystemMode() {
  switch (systemMode) {
    case 1: mode1(); break;
    case 2: mode2(); break;
    default: mode3(); break;
  }
}

// mode 1: single color mode - pressing a button selects that color exclusively
void mode1() {
  stateChanged = false;

  // check for button presses and set corresponding color bit 
  if (btn.rPressed) {
    btn.rPressed = false;
    if (colorStateMask != RED_BIT) {
      colorStateMask = RED_BIT;
      stateChanged = true;
      lastActivity = millis();
    }
  } else if (btn.gPressed) {
    btn.gPressed = false; 
    if (colorStateMask != GREEN_BIT) {
      colorStateMask = GREEN_BIT;
      stateChanged = true;
      lastActivity = millis();
    }
  } else if (btn.bPressed) {
    btn.bPressed = false; 
    if (colorStateMask != BLUE_BIT) {
      colorStateMask = BLUE_BIT;
      stateChanged = true;
      lastActivity = millis();
    }
  } 

  // auto-sleep after 5 seconds of no activity
  if (millis() - lastActivity >= sleepTimer && colorStateMask != 0) {
    colorStateMask = 0;
    lastActivity = millis();
  }

  sendCurrentState();
}

// mode 2: independent toggle - each button toggles its color on/off
void mode2() {
  stateChanged = false;

  // debounce timers
  static unsigned long rToggleLockoutUntil = 0;
  static unsigned long gToggleLockoutUntil = 0;
  static unsigned long bToggleLockoutUntil = 0;

  // attempt to toggle each color independently with debounce protection
  stateChanged |= toggleColorBit(btn.rPressed, rToggleLockoutUntil, RED_BIT);
  stateChanged |= toggleColorBit(btn.gPressed, gToggleLockoutUntil, GREEN_BIT);
  stateChanged |= toggleColorBit(btn.bPressed, bToggleLockoutUntil, BLUE_BIT);

  sendCurrentState();
}

// mode 3: hold to activate - led is on while button is held, off when released
void mode3() {
  const uint8_t newStateMask = 
    (btn.r ? RED_BIT : 0) | 
    (btn.g ? GREEN_BIT : 0) | 
    (btn.b ? BLUE_BIT : 0);

  stateChanged = (newStateMask != colorStateMask);
  colorStateMask = newStateMask;

  sendCurrentState();
}

// send current led states
void sendCurrentState() {
  uint8_t red, green, blue;
  getColorStates(red, green, blue);

  // send periodic updates
  if (millis() - lastSend >= sendInterval) {
    lastSend = millis();
    sendCommand(red, green, blue);
    sendInterval = 500 + random(-50, 51);
  }

  // send immediate update if state changed
  if (stateChanged) {
    printf("State changed to R:%u G:%u B:%u ", red, green, blue);
    sendCommand(red, green, blue);
  }
}

// send command frame to receiver
void sendCommand(uint8_t red, uint8_t green, uint8_t blue) {
  cyclesElapsed++;
  pulseActivityLed(10);
  Frame frame = { CONTROLLER_ADDRESS, red, green, blue, brightnessOptions[brightnessIndex] };
  sendFrame(frame);
  // printf("Sent state R:%u G:%u B:%u\n", red, green, blue);
}

// display receiver echo states on controller rgb led
void processEcho() {
  while (RFSerial.available()) {
    Frame f;
    if (readFrame(f)) {
      if (f.device == RECEIVER_ADDRESS) {
        if (isValidBoolByte(f.red) && isValidBoolByte(f.green) && isValidBoolByte(f.blue) && !blinking) {
          // valid frame - update LED
          updateRGBLED(f.red, f.green, f.blue);
        } else {
          // invalid color values (receiver sent error indicator)
          cyclesElapsed = 0;
        }
      }
    } else {
      // bad CRC or frame read error
      cyclesElapsed = 0;
    }
  }
}

// blink purple led when receiver error is detected
void errorBlink() {
  // check if an error occured within last 2 cycles
  
  if (cyclesElapsed < 2 ) {
    blinking = true;
    if ((millis() % 300) < 150) {
      updateRGBLED(1, 0, 1);
    } else {
      updateRGBLED(0, 0, 0);
    }
  }
  else {
    blinking = false;
  }
}

// update rgb led based on receiver state
void updateRGBLED(uint8_t red, uint8_t green, uint8_t blue) {
  analogWrite(rPin, red ? 1 : 0);
  analogWrite(gPin, green ? 1 : 0);
  analogWrite(bPin, blue ? 1 : 0);
}
