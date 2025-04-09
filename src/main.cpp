#include <Arduino.h>
#include "receiver.h"
#include "controller.h"
#include "common.h"
//----------------------------------------------------
// Select the role of this device:
// Uncomment only one of the following lines.
// You can also set these as build flags in platformio.ini.
#define CONTROLLER


void setup()
{
  // Start USB Serial for debugging
  Serial.begin(9600);
  delay(1000);

  // Begin RF Serial on Serial1 (UART)
  RFSerial.begin(9600, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);

  // Set up RGB LED pins
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  // Set up additional LED pin
  pinMode(ledPin, OUTPUT);

#ifdef CONTROLLER
  setupController();
#else
  setupReceiver();
#endif
}

//----------------------------------------------------
void loop()
{
#ifdef CONTROLLER
  loopController();
#else
  loopReceiver();
#endif
}
