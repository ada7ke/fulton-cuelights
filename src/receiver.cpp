#include "receiver.h"
#include "common.h"

// pwm settings
const int freq = 5000;
const int res = 8;
static uint8_t brightness = 15;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;



static unsigned long lastMessage = 0;
static Frame lastEcho = { RECEIVER_ADDRESS, 0, 0, 0, brightness };
static unsigned long lastSend = 0;

void setupReceiver()
{
  randomSeed(micros());

  ledcSetup(redChannel, freq, res);
  ledcSetup(greenChannel, freq, res);
  ledcSetup(blueChannel, freq, res);

  ledcAttachPin(redLED, redChannel);
  ledcAttachPin(greenLED, greenChannel);
  ledcAttachPin(blueLED, blueChannel);

  pinMode(LED_PIN, OUTPUT);

  updateLEDs(0, 0, 0);
  digitalWrite(LED_PIN, HIGH);

  lastMessage = millis();
  lastEcho = { RECEIVER_ADDRESS, 0, 0, 0, brightness };
  lastSend = 0;
}

void loopReceiver()
{
  serviceActivityLed();
  
  processCommand();
  sendIntervaledEcho();
  timeoutReceiver();
}

// process incoming command frames
void processCommand() {
  while (RFSerial.available()) {
    Frame f;
    if (readFrame(f)) {
      if (f.device == CONTROLLER_ADDRESS) {
        if (isValidBoolByte(f.red) && isValidBoolByte(f.green) && isValidBoolByte(f.blue)) {
          // valid frame - update leds and send echo
          pulseActivityLed(10);
          lastMessage = millis();
          brightness = f.brightness;
          updateLEDs(f.red, f.green, f.blue);
          // send echo only if state changed
          if (lastEcho.red != f.red || lastEcho.green != f.green || lastEcho.blue != f.blue || lastEcho.brightness != brightness) {
            lastEcho = { RECEIVER_ADDRESS, f.red, f.green, f.blue, brightness };
            sendFrame(lastEcho);
          }
          lastEcho = { RECEIVER_ADDRESS, f.red, f.green, f.blue, brightness };
        } else {
          // send error frame with invalid values (2 = error indicator)
          printf("Error: Invalid color values in frame\n");
          Frame errorFrame = { RECEIVER_ADDRESS, 2, 2, 2, brightness };
          sendFrame(errorFrame);
        }
      }
    } else {
      // send error frame on CRC/read failure
      printf("Error: Failed to read frame\n");
      Frame errorFrame = { RECEIVER_ADDRESS, 2, 2, 2, brightness };
      sendFrame(errorFrame);
    }
  }
}

// send echo at intervals
void sendIntervaledEcho() {
  if (millis() - lastSend >= sendInterval) {
    lastSend = millis();
    sendFrame(lastEcho);
    sendInterval = 500 + random(-50, 51);
  }
}

// no message timeout
void timeoutReceiver() {
  if (millis() - lastMessage > 5000) {
    lastMessage = millis();
    updateLEDs(0, 0, 0);

    lastEcho = { RECEIVER_ADDRESS, 0, 0, 0, brightness };
    sendFrame(lastEcho);

    printf("No message timeout, turning off LEDs\n");
  }
}

// update led states
void updateLEDs(uint8_t red, uint8_t green, uint8_t blue) {
  auto setLEDs = [](uint8_t r, uint8_t g, uint8_t b) {
    ledcWrite(redChannel, r);
    ledcWrite(greenChannel, g);
    ledcWrite(blueChannel, b);
  };

  uint8_t r = red ? brightness : 0;
  uint8_t g = green ? brightness : 0;
  uint8_t b = blue ? brightness : 0;

  setLEDs(r, g, b);
  printf("Updated LEDs - Brightness: %u | R: %u, G: %u, B: %u\n", brightness, r, g, b);
}

