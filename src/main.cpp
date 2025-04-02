#include <Arduino.h>

#define LED_PIN 8       // Onboard LED on ESP32-C3 SuperMini
#define SWITCH_PIN 3    // Connect your two-way switch here

void setup() {
  pinMode(LED_PIN, OUTPUT);           // Set the LED pin as output
  pinMode(SWITCH_PIN, INPUT_PULLUP);  // Set the switch pin as input with pull-up
}

void loop() {
  // Read the switch state (LOW when pressed due to pull-up)
  if (digitalRead(SWITCH_PIN) == LOW) {
    digitalWrite(LED_PIN, HIGH);  // Switch is pressed: turn LED on
  } else {
    digitalWrite(LED_PIN, LOW);   // Switch is not pressed: turn LED off
  }
}
