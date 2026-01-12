/* TODO:
 - ota uploads
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
  Serial.flush();

  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  deviceRole = autoDetectRole();
  if (deviceRole == ROLE_CONTROLLER) {
    RFSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // (i soldered these wrong)
    delay(100);
    printf("Auto detected role: CONTROLLER\n");
    setupController();
  } else {
    RFSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); 
    delay(100);
    printf("Auto detected role: RECEIVER\n");
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
