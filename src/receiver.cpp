#include "receiver.h"
#include "common.h"

void setupReceiver()
{
    updateRGBLED('L');
}

void loopReceiver()
{
    // --- Receiver Code ---
  if (RFSerial.available())
  {
    char inChar = RFSerial.read();
    // Debug output
    Serial.print("Received: ");
    Serial.println(inChar);
    // Update the RGB LED based on the received mode
    updateRGBLED(inChar);
  }
  delay(10);
}

// #include <Arduino.h>

// // --- Pin Definitions ---
// // RGB LED pins (common cathode; HIGH = ON)
// #define rPin 2    // Red channel
// #define gPin 1    // Green channel
// #define bPin 0    // Blue channel

// // RF module UART pins (adjust as needed)
// #define RF_TX_PIN 20  // (Unused here if receiver only reads)
// #define RF_RX_PIN 21  // TX pin from RF module

// // We'll use Serial1 for RF module communication.
// HardwareSerial RFSerial(1);

// // Function to update the onboard RGB LED based on received mode
// void updateLED(char modeChar) {
//   switch (modeChar) {
//     case 'L':
//       // Red
//       analogWrite(rPin, 255);
//       analogWrite(gPin, 0);
//       analogWrite(bPin, 0);
//       break;
//     case 'N':
//       // Yellow: full red, reduced green for a warmer tone
//       analogWrite(rPin, 255);
//       analogWrite(gPin, 150);  // Adjust as needed for the desired yellow
//       analogWrite(bPin, 0);
//       break;
//     case 'R':
//       // Green
//       analogWrite(rPin, 0);
//       analogWrite(gPin, 255);
//       analogWrite(bPin, 0);
//       break;
//     default:
//       // If an unknown message is received, turn off the LED
//       analogWrite(rPin, 0);
//       analogWrite(gPin, 0);
//       analogWrite(bPin, 0);
//       break;
//   }
// }

// void setup() {
//   // For debugging on USB Serial
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("Receiver starting...");

//   // Begin RF Serial on Serial1 (UART)
//   RFSerial.begin(9600, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);

//   // Configure RGB LED pins as outputs
//   pinMode(rPin, OUTPUT);
//   pinMode(gPin, OUTPUT);
//   pinMode(bPin, OUTPUT);

//   // Initialize LED off or to a default state (e.g., yellow)
//   updateLED('N');
// }

// void loop() {
//   // Check if data is available on the RF Serial
//   if (RFSerial.available()) {
//     char inChar = RFSerial.read();

//     // Debug print the received character
//     Serial.print("Received: ");
//     Serial.println(inChar);

//     // Update the RGB LED based on the received message
//     updateLED(inChar);
//   }

//   delay(10);  // Short delay to prevent overwhelming the loop
// }
