#include <Arduino.h>
#include "common.h"

// Use HardwareSerial (Serial1) for the RF module
// HardwareSerial RFSerial(3);

// Function to update the RGB LED based on mode character
// 'L' = Left (red), 'N' = Neither (yellow), 'R' = Right (green)
void updateRGBLED(char modeChar)
{
  switch (modeChar)
  {
  case 'L':
    analogWrite(rPin, 255);
    analogWrite(gPin, 0);
    analogWrite(bPin, 0);
    break;
  case 'N':
    analogWrite(rPin, 255);
    analogWrite(gPin, 150); // Adjust this value for a warmer yellow tone
    analogWrite(bPin, 0);
    break;
  case 'R':
    analogWrite(rPin, 0);
    analogWrite(gPin, 255);
    analogWrite(bPin, 0);
    break;
  default:
    // Turn off LED for any unknown value
    analogWrite(rPin, 0);
    analogWrite(gPin, 0);
    analogWrite(bPin, 255);
    break;
  }
}