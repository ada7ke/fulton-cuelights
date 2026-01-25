#include <Arduino.h>
#include "receiver.h"
#include "controller.h"
#include "common.h"
#include "WifiAP.hpp"

// ground pin for controller
#define ROLE_DETECT_PIN 5

enum DeviceRole {
  ROLE_CONTROLLER,
  ROLE_RECEIVER
};

DeviceRole deviceRole;

DeviceRole autoDetectRole() {
  pinMode(ROLE_DETECT_PIN, INPUT_PULLUP);

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

  // setup_wifi(); disabled ota

  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // detect device type
  deviceRole = autoDetectRole();
  if (deviceRole == ROLE_CONTROLLER) {
    RFSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); 
    delay(10);
    printf("Auto detected role: CONTROLLER\n");
    setupController();
  } else {
    RFSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); 
    delay(10);
    printf("Auto detected role: RECEIVER\n");
    setupReceiver();
  }
}

void loop()
{
  // ArduinoOTA.handle();
  // ElegantOTA.loop();
  // dnsServer.processNextRequest();
  
  // run loop according to device type
  if (deviceRole == ROLE_CONTROLLER) {
    loopController();
  } else {
    loopReceiver();
  }
}
