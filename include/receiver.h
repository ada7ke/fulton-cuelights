#pragma once

#define redLED 7
#define yellowLED 6
#define greenLED 10

enum State
{
  WAIT_START,
  READ_MODE,
  READ_CHECKSUM,
  WAIT_END
};

void setupReceiver();
void loopReceiver();
void updateLEDs(char);