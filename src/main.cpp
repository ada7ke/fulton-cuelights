/* TODO: 
 - if no signals recieved for 10 seconds, turn on all leds off
 - convert sleepTimer to use millis()
 - update receiver echo to also use crc 
 - long button press features
    * brightness
    * turn all leds off

*/

#include <Arduino.h>
#include "receiver.h"
#include "controller.h"
#include "common.h"

#define ROLE_DETECT_PIN 5

enum DeviceRole {
  ROLE_CONTROLLER,
  ROLE_RECEIVER
};

DeviceRole deviceRole;

DeviceRole autoDetectRole() {
  pinMode(ROLE_DETECT_PIN, INPUT_PULLUP);
  delay(10); 

  if (digitalRead(ROLE_DETECT_PIN) == LOW) {
    return ROLE_CONTROLLER;
  } else {
    return ROLE_RECEIVER;
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  deviceRole = autoDetectRole();
  // Begin RF Serial on Serial1 (UART)
  if (deviceRole == ROLE_CONTROLLER) {
    RFSerial.begin(9600, SERIAL_8N1, 20, 21); // (i soldered these wrong)
    Serial.println("Auto detected role: CONTROLLER");
    setupController();
  } else {
    RFSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); 
    Serial.println("Auto detected role: RECEIVER");
    setupReceiver();
  }
}

void loop()
{
  if (deviceRole == ROLE_CONTROLLER) {
    loopController();
  } else {
    loopReceiver();
  }
}
