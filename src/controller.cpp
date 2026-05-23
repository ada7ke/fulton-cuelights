#include <Arduino.h>
#include "controller.h"
#include "common.h"

int systemMode = 1; // default system mode 1: single color mode

static unsigned long lastSend = 0;
static unsigned long lastActivity = 0;

struct ButtonState {
  bool r = false;
  bool b = false;
  bool g = false;
  bool rPressed = false;
  bool bPressed = false;
  bool gPressed = false;
};
static ButtonState btn;

bool stateChanged = false;
static constexpr unsigned long longHoldDuration = 3000UL;
static const uint8_t brightnessOptions[] = { 5, 15, 30 }; // edit values to change brightness presets
static uint8_t brightnessIndex = 1;
static constexpr size_t brightnessOptionsCount = sizeof(brightnessOptions) / sizeof(brightnessOptions[0]);
static constexpr unsigned long mode2ToggleDebounceMs = 200;

static uint8_t colorStateMask = 0;
static constexpr uint8_t BLUE_BIT = 0x01;
static constexpr uint8_t GREEN_BIT = 0x02;
static constexpr uint8_t RED_BIT = 0x04;

static inline uint8_t boolToByte(bool v) { return v ? 1 : 0; }

static inline void getColorStates(uint8_t &red, uint8_t &green, uint8_t &blue) {
  red = boolToByte(colorStateMask & RED_BIT);
  green = boolToByte(colorStateMask & GREEN_BIT);
  blue = boolToByte(colorStateMask & BLUE_BIT);
}

void setupController() {
  randomSeed(micros());

  pinMode(rButtonPin, INPUT_PULLUP);
  pinMode(bButtonPin, INPUT_PULLUP);
  pinMode(gButtonPin, INPUT_PULLUP);

  updateRGBLED(0, 0, 0);
  digitalWrite(LED_PIN, HIGH);
  lastActivity = millis();
  printf("Controller setup complete\n");
}

void loopController() {
  //printf("A serviceActivityLed; ");
  serviceActivityLed();

  //printf("B detectButtonChange; ");
  detectButtonChange();

  //printf("C handleControllerShortcuts; ");
  handleControllerShortcuts();

  //printf("D runSystemMode; ");
  runSystemMode();

  //printf("E processEcho; ");
  processEcho();

  //printf("G loopController done; ");
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

// update button events
void updateButtonEvents(bool now, bool &last, bool &pressedEvent) {
  pressedEvent = (now && !last);
  last = now;
}

// handle button state changes and long-press detection
void detectButtonChange() {
  static bool lastR = false;
  static bool lastB = false;
  static bool lastG = false;

  btn.rPressed = false;
  btn.gPressed = false;
  btn.bPressed = false;

  btn.r = (digitalRead(rButtonPin) == LOW);
  btn.g = (digitalRead(gButtonPin) == LOW);
  btn.b = (digitalRead(bButtonPin) == LOW);
  //printf("Buttons state R:%d G:%d B:%d\n", btn.r, btn.g, btn.b);

  updateButtonEvents(btn.r, lastR, btn.rPressed);
  updateButtonEvents(btn.g, lastG, btn.gPressed);
  updateButtonEvents(btn.b, lastB, btn.bPressed);
}

// handle long hold actions
void handleControllerShortcuts() {
  static unsigned long SCStart = 0;
  static bool SCHandled = false;

  bool modeSC = btn.b && btn.g;        // blue + green
  bool brightnessSC = btn.r && btn.g;  // red + green
  // no shortcut is being held
  if (!modeSC && !brightnessSC) {
    SCStart = 0;
    SCHandled = false;
    return;
  }

  // start timing the shortcut
  if (SCStart == 0) {
    SCStart = millis();
    SCHandled = false;
  }

  // not held long enough yet
  if (SCHandled || millis() - SCStart < longHoldDuration) {
    return;
  }

  // prevent normal color button behavior from also firing
  btn.rPressed = false;
  btn.gPressed = false;
  btn.bPressed = false;

  SCHandled = true;
  if (modeSC) {
    cycleSystemMode();
  } else if (brightnessSC) {
    cycleBrightness();
  }
}

// blink blue rgb led on long hold button events
void blink(int times, int red, int green, int blue) {
  for (int i = 0; i < times; ++i) {
    updateRGBLED(red, green, blue);
    delay(200);
    updateRGBLED(0, 0, 0);
    delay(150);
  }
}

// cycle between led brightness levels
void cycleBrightness() {
  brightnessIndex = (brightnessIndex + 1) % brightnessOptionsCount;
  printf("Brightness level changed to: %u\n", brightnessOptions[brightnessIndex]);
  blink(3, 0, 0, 1); // blink blue
  stateChanged = true;
  lastActivity = millis();
}

// cycle between system modes 1, 2, and 3
void cycleSystemMode() {
  systemMode = (systemMode % 3) + 1;
  printf("System mode changed to: %d\n", systemMode);
  blink(3, 0, 0, 1); // blink blue
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
  int sleepTimer = 5000;
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
    sendInterval = 500 + random(-jitter, jitter);
    //printf("Periodic update sent with state R:%u G:%u B:%u\n", red, green, blue);
  }

  // send immediate update if state changed
  if (stateChanged) {
    printf("State changed to R:%u G:%u B:%u \n", red, green, blue);
    sendCommand(red, green, blue);
  }
}

// send command frame to receiver
void sendCommand(uint8_t red, uint8_t green, uint8_t blue) {
  pulseActivityLed(10);
  Frame frame = { CONTROLLER_ADDRESS, red, green, blue, brightnessOptions[brightnessIndex] };
  sendFrame(frame);
  // printf("Sent state R:%u G:%u B:%u\n", red, green, blue);
}

// display receiver echo states on controller rgb led
void processEcho() {
  int framesProcessed = 0;

  while (RFSerial.available() && framesProcessed < 3) {
    Frame f;
    ReadFrameResult result = readFrame(f);

    if (result == FRAME_OK) {
      framesProcessed++;

      //printf("RX echo dev:%u R:%u G:%u B:%u brightness:%u\n", f.device, f.red, f.green, f.blue, f.brightness);

      if (f.device == RECEIVER_ADDRESS) {
        if (isValidBoolByte(f.red) && isValidBoolByte(f.green) && isValidBoolByte(f.blue)) {
          if (f.blue) {
            updateRGBLED(1, 1, 0);
          }
          else {
            updateRGBLED(f.red, f.green, f.blue);
          }
        } else {
          printf("Receiver sent invalid echo values\n");
          blink(5, 1, 0, 1);
        }
      }

    } else if (result == FRAME_ERROR) {
      framesProcessed++;
      printf("Bad echo frame or CRC\n");
      blink(5, 1, 0, 1);
    } else {
      break;  // FRAME_NONE. partial frame, wait for more bytes
    }
  }
}

// update rgb led based on receiver state
void updateRGBLED(uint8_t red, uint8_t green, uint8_t blue) {
  // printf("Updating RGB LED to R:%u G:%u B:%u\n", red, green, blue);
  analogWrite(rPin, red ? 255 : 0);
  analogWrite(gPin, green ? 255 : 0);
  analogWrite(bPin, blue ? 255 : 0);
}