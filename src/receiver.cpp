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

static Frame lastEcho = { RECEIVER_ADDRESS, 0, 0, 0, brightness };
static unsigned long lastSend = 0;
static unsigned long sendInterval = 500;

void setupReceiver()
{
  randomSeed(micros());

  ledcSetup(redChannel, freq, res);
  ledcSetup(yellowChannel, freq, res);
  ledcSetup(greenChannel, freq, res);

  ledcAttachPin(redLED, redChannel);
  ledcAttachPin(yellowLED, yellowChannel);
  ledcAttachPin(greenLED, greenChannel);

  pinMode(LED_PIN, OUTPUT);

  updateLEDs(0, 0, 0);
  digitalWrite(LED_PIN, HIGH);

  lastMessage = millis();
  lastEcho = { RECEIVER_ADDRESS, 0, 0, 0, brightness };
  lastSend = 0;
}

void loopReceiver()
{
  Frame f;
  if (readFrame(f)) {
      if (f.device == CONTROLLER_ADDRESS &&
          isValidBoolByte(f.red) && isValidBoolByte(f.yellow) && isValidBoolByte(f.green)) {
        digitalWrite(LED_PIN, LOW);  
        lastMessage = millis();
        brightness = f.brightness;

        updateLEDs(f.red, f.yellow, f.green);

        lastEcho = { RECEIVER_ADDRESS, f.red, f.yellow, f.green, brightness };
      }
  }

  if (millis() - lastSend >= sendInterval) {
    lastSend = millis();
    sendFrame(lastEcho);
    sendInterval = 500 + random(-50, 51);
  }

  // no message timeout
  if (millis() - lastMessage > 5000) {
    lastMessage = millis();
    updateLEDs(0, 0, 0);

    lastEcho = { RECEIVER_ADDRESS, 0, 0, 0, brightness };
    sendFrame(lastEcho);

    printf("No message timeout, turning off LEDs\n");
  }

  delay(10);
  digitalWrite(LED_PIN, HIGH);
  delay(100);
}

void updateLEDs(uint8_t red, uint8_t yellow, uint8_t green) {
  auto setLEDs = [](uint8_t r, uint8_t y, uint8_t g) {
    ledcWrite(redChannel, r);
    ledcWrite(yellowChannel, y);
    ledcWrite(greenChannel, g);
  };

  uint8_t r = red ? brightness : 0;
  uint8_t y = yellow ? brightness : 0;
  uint8_t g = green ? brightness : 0;

  setLEDs(r, y, g);
  printf("Updated LEDs - Brightness: %u | R: %u, Y: %u, G: %u\n", brightness, r, y, g);
}
