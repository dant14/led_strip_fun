#include "ledFuncs.h"
#include "nvsFuncs.h"
#include "buttons.h"
#include "timers.h"

void setup()
{
//  EEPROM.write((uint16_t)2, 15);
  int brightness;
  byte ledModeState;
  // Set button input pin
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BR_UP_PIN, INPUT_PULLUP);
  pinMode(BR_DN_PIN, INPUT_PULLUP);
  delay(1000);
//  pinMode(7, OUTPUT);
//  digitalWrite(7, HIGH);
  Serial.begin(115200);
  Serial.println("hi!");
  ledModeState = getEepromState();
  setLedModeState(ledModeState);
  brightness = getEepromBright();
  if((ledModeState == 0xFF) || (brightness >= 10))//initial mcu conditions
  {
    setEepromState(0);
    setEepromBright(8);//max 10
  }
  setupLeds();
  blankAllLeds();
  Serial.print("Current led mode state is: ");
  Serial.println(getLedModeState());
  setSingleShotFlag(1);
//  attachInterrupt(digitalPinToInterrupt(INT_PIN), pin_ISR, RISING);
  for ( int i = 0; i < 1000; i++ )
  {
    // 1 is for 'test' mode, 0 is for normal button scanning; fixes issues with initial weird behaviours
    brightnessUPmodeButtonTest(1);
    brightnessDNmodeButtonTest(1);
    modeButtonTest(1);
    delay(1);
  }
  Serial.print("Setup END");
}

void loop() 
{
  brightnessUPmodeButtonTest(0);
  brightnessDNmodeButtonTest(0);
  modeButtonTest(0);
  if(getSuspendFlag() != 1)
  {
    stateMachine();
  }
  else keepTimerAlive(long(TIMERDELAY));
}
