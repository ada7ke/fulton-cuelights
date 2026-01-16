#include "receiver.h"
#include "common.h"

// pwm settings
const int freq = 5000;
const int res = 8;
static uint8_t brightness = 5; // (1/5/10/15)
const int redChannel = 0;
const int yellowChannel = 1;
const int greenChannel = 2;

static unsigned long lastMessage = 0;

void setupReceiver()
{
  ledcSetup(redChannel, freq, res);
  ledcSetup(yellowChannel, freq, res);
  ledcSetup(greenChannel, freq, res);

  ledcAttachPin(redLED, redChannel);
  ledcAttachPin(yellowLED, yellowChannel);
  ledcAttachPin(greenLED, greenChannel);

  pinMode(LED_PIN, OUTPUT);

  updateLEDs(Mode::X, false);
  digitalWrite(LED_PIN, HIGH);
}

void loopReceiver()
{
  Frame f;
  if (readFrame(f)) {
      if (f.device == CONTROLLER_ADDRESS && isValidModeByte((uint8_t)f.mode)) {
        digitalWrite(LED_PIN, LOW);  
        lastMessage = millis();
        brightness = f.brightness;
        updateLEDs(static_cast<Mode>(f.mode), f.red);
        Frame echo = { RECEIVER_ADDRESS, f.mode, f.red, brightness };
        sendFrame(echo);
      }
  }

  if (millis() - lastMessage > 5000) {
    lastMessage = millis();
    updateLEDs(Mode::B, false);
    Frame timeoutFrame = { RECEIVER_ADDRESS, static_cast<uint8_t>(Mode::B), 0, brightness };
    sendFrame(timeoutFrame);
    printf("No message timeout, turning off LEDs\n");
  }

  delay(10);
  digitalWrite(LED_PIN, HIGH);
  delay(100);
}

void updateLEDs(Mode mode, bool mode_r) {
  auto setLEDs = [](uint8_t r, uint8_t y, uint8_t g) {
    ledcWrite(redChannel, r);
    ledcWrite(yellowChannel, y);
    ledcWrite(greenChannel, g);
  };

  uint8_t r = mode_r ? brightness : 0;
  uint8_t y = (mode == Mode::Y) ? brightness : 0;
  uint8_t g = (mode == Mode::G) ? brightness : 0;

  setLEDs(r, y, g);
  printf("Updated LEDs - Duty: %u | R: %u, Y: %u, G: %u\n", brightness, r, y, g);
}
