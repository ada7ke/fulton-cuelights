#include <Arduino.h>
#include "receiver.h"
#include "controller.h"
#include "common.h"

//----------------------------------------------------
// Define the role detection pin
#define ROLE_DETECT_PIN 5

enum DeviceRole {
  ROLE_CONTROLLER,
  ROLE_RECEIVER
};

DeviceRole deviceRole;

//----------------------------------------------------
// Function to auto-detect the role using GPIO5
DeviceRole autoDetectRole() {
  pinMode(ROLE_DETECT_PIN, INPUT_PULLUP);
  delay(10);  // Let the pull-up settle

  if (digitalRead(ROLE_DETECT_PIN) == LOW) {
    return ROLE_CONTROLLER;
  } else {
    return ROLE_RECEIVER;
  }
}

//----------------------------------------------------
void setup()
{
  // Start USB Serial for debugging
  Serial.begin(115200);
  delay(1000);

  // Begin RF Serial on Serial1 (UART)
  RFSerial.begin(9600, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);

  // Set up RGB LED pins
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  // Set up additional LED pin
  pinMode(ledPin, OUTPUT);

  // Detect the role based on GPIO5
  deviceRole = autoDetectRole();
  if (deviceRole == ROLE_CONTROLLER) {
    Serial.println("Auto detected role: CONTROLLER");
    setupController();
  } else {
    Serial.println("Auto detected role: RECEIVER");
    setupReceiver();
  }
}

//----------------------------------------------------
void loop()
{
  if (deviceRole == ROLE_CONTROLLER) {
    loopController();
  } else {
    loopReceiver();
  }
}
