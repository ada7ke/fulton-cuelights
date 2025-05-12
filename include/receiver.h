#pragma once

#define yellowLED 7
#define greenLED 9


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