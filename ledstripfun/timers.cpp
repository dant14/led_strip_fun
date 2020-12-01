#include "Arduino.h"
#include "timers.h"
#include "buttons.h"

static long initTimeStore;

//this is for preventing the led loop from interrupting the brightness buttons in long press mode
void fireTimer()
{
  initTimeStore = millis();
}

//same
void keepTimerAlive(long timerLength)
{
  if((millis() - timerLength) > initTimeStore) setSuspendFlag(0);
}

