#include "Arduino.h"
#include "ledFuncs.h"
#include "nvsFuncs.h"
#include "buttons.h"
#include "timers.h"

#define debounce              20 // ms debounce period to prevent flickering when pressing or releasing the button
#define holdTime              1000 // ms hold period: how long to wait for press+hold event
#define BRIGHTNESS_HOLD_TIME  400 // ms hold period: how long to wait for press+hold event


static byte ledModeState  = 0;
static int singleShotFlag = 0;
static int suspendFlag    = 0;

// Button variables
static int buttonVal = 0; // value read from button
static int brightnessUPbuttonVal = 0;
static int brightnessDNbuttonVal = 0;

static int buttonLast = 0; // buffered value of the button's previous state
static int brightnessUPbuttonLast = 0;
static int brightnessDNbuttonLast = 0;

static long btnDnTime; // time the button was pressed down
static long brightnessUPbtnDnTime;
static long brightnessDNbtnDnTime;

static long btnUpTime; // time the button was released
static long brightnessUPbtnUpTime;
static long brightnessDNbtnUpTime;

static boolean ignoreUp = false; // whether to ignore the button release because the click+hold was triggered
static boolean brightnessUPignoreUp = false;
static boolean brightnessDNignoreUp = false;

void shortPress();
void longPress();
void brightUPshort();
void brightDNshort();
void brightUPlong();
void brightDNlong();
void incrementState();

int getSingleShotFlag()
{
  return singleShotFlag;
}

int getLedModeState()
{
  return ledModeState;
}

void setSingleShotFlag(int val)
{
  singleShotFlag = val;
}

void setLedModeState(int val)
{
  ledModeState = val;
}

void setSuspendFlag(int val)
{
  suspendFlag = val;
}

int getSuspendFlag()
{
  return suspendFlag;
}

void modeButtonTest(int initConditionTest)//(int initConditionTest)
{
  // Read the state of the button
  buttonVal = digitalRead(MODE_BUTTON_PIN);
  
  // Test for button pressed and store the down time
  if (buttonVal == LOW && buttonLast == HIGH && (millis() - btnUpTime) > long(debounce))
  {
    btnDnTime = millis();
  }
  
  // Test for button release and store the up time
  if (buttonVal == HIGH && buttonLast == LOW && (millis() - btnDnTime) > long(debounce))
  {
    if (ignoreUp == false && initConditionTest == 0) 
    {
      shortPress();
    }
    else ignoreUp = false;
    btnUpTime = millis();
  }
  
  
  // Test for button held down for longer than the hold time
  if (buttonVal == LOW && (millis() - btnDnTime) > long(holdTime))
  {
    if (initConditionTest == 0)
    {
      longPress();
    }
    ignoreUp = true;
    btnDnTime = millis();
  }
  buttonLast = buttonVal;
}

void brightnessUPmodeButtonTest(int initConditionTest)
{
  // Read the state of the button
  brightnessUPbuttonVal = digitalRead(BR_UP_PIN);
  
  // Test for button pressed and store the down time
  if (brightnessUPbuttonVal == LOW && brightnessUPbuttonLast == HIGH && (millis() - brightnessUPbtnUpTime) > long(debounce))
  {
  brightnessUPbtnDnTime = millis();
  }
  
  // Test for button release and store the up time
  if (brightnessUPbuttonVal == HIGH && brightnessUPbuttonLast == LOW && (millis() - brightnessUPbtnDnTime) > long(debounce))
  {
  if (brightnessUPignoreUp == false && initConditionTest == 0)
  {
    Serial.println("button up short pressed!");
    brightUPshort();
  }
  else brightnessUPignoreUp = false;
  brightnessUPbtnUpTime = millis();
  }
  
  // Test for button held down for longer than the hold time
  if (brightnessUPbuttonVal == LOW && (millis() - brightnessUPbtnDnTime) > long(BRIGHTNESS_HOLD_TIME))
  {
//  Serial.println("LONG button UP LONG pressed!");
  brightUPlong();
  setSuspendFlag(1);
  fireTimer();
  brightnessUPignoreUp = true;
  brightnessUPbtnDnTime = millis();
  }
  brightnessUPbuttonLast = brightnessUPbuttonVal;
}

void brightnessDNmodeButtonTest(int initConditionTest)
{
  // Read the state of the button
  brightnessDNbuttonVal = digitalRead(BR_DN_PIN);
  
  // Test for button pressed and store the down time
  if (brightnessDNbuttonVal == LOW && brightnessDNbuttonLast == HIGH && (millis() - brightnessDNbtnUpTime) > long(debounce))
  {
  brightnessDNbtnDnTime = millis();
  }
  
  // Test for button release and store the up time
  if (brightnessDNbuttonVal == HIGH && brightnessDNbuttonLast == LOW && (millis() - brightnessDNbtnDnTime) > long(debounce))
  {
  if (brightnessDNignoreUp == false && initConditionTest == 0)
  {
    Serial.println("down button short pressed!");
    brightDNshort();
  }
  else brightnessDNignoreUp = false;
  brightnessDNbtnUpTime = millis();
  }
  
  // Test for button held down for longer than the hold time
  if (brightnessDNbuttonVal == LOW && (millis() - brightnessDNbtnDnTime) > long(BRIGHTNESS_HOLD_TIME))
  {
//  Serial.println("LONG button DOWN LONG pressed!");
  brightDNlong();
  setSuspendFlag(1);
  fireTimer();
  brightnessDNignoreUp = true;
  brightnessDNbtnDnTime = millis();
  }
  brightnessDNbuttonLast = brightnessDNbuttonVal;
}

void shortPress()
{
  Serial.println("shortPress");
  if(ledModeState == MAX_MODES)
  {
    Serial.println("state 6 which is an off-transition kinda thing");
    ledModeState = getEepromState();
    if(ledModeState == MAX_MODES)
    {
      incrementState();
      Serial.print("incremented state: ");
    }
    blankAllLeds();
    Serial.println(ledModeState);
  }
  else
  {
    incrementState();
    setEepromState(ledModeState);
    Serial.print("else state: ");
    Serial.println(ledModeState);
    Serial.println();
    blankAllLeds();
    if(ledModeState != MAX_MODES) modeIndicator();
  }
  singleShotFlag = 1;
}

void incrementState()
{
  if(ledModeState < MAX_MODES)
  {
    ledModeState++;
//    Serial.println();
  }
  else
  {
    ledModeState = 1;
    modeIndicator();
  }
}

void longPress()
{
  ledModeState = MAX_MODES;
  Serial.println("longPress");
  delay(20);
}

void brightUPshort()
{
  Serial.println("brightUPshort");
  byte brightness = getEepromBright();
  if(brightness == BRIGHTNESS_LEVELS)
  {
    Serial.println("MAX BRIGHTNESS!");
  }
  else
  {
    brightness = ledFuncs_incrBrightByStep(brightness);
    setEepromBright(brightness);
    delay(300);
  }
}

void brightDNshort()
{
  Serial.println("brightDNshort");
  byte brightness = getEepromBright();
  if(brightness == 0)
  {
    Serial.println("MIN BRIGHTNESS!");
  }
  else
  {
    brightness = ledFuncs_decBrightByStep(brightness);
    setEepromBright(brightness);
    delay(300);
  }
}

void brightUPlong()
{
  Serial.println("brightUPlong");
  byte brightness = getEepromBright();
  if(brightness == BRIGHTNESS_LEVELS)
  {
    Serial.println("MAX BRIGHTNESS!");
  }
  else
  {
    brightness = ledFuncs_incrBrightByStep(brightness);
    setEepromBright(brightness);
  }
}

void brightDNlong()
{
  Serial.println("brightDNlong");
  byte brightness = getEepromBright();
  if(brightness == 0)
  {
    Serial.println("MIN BRIGHTNESS!");
  }
  else
  {
    brightness = ledFuncs_decBrightByStep(brightness);
    setEepromBright(brightness);
  }
}

void stateMachine()
{
  switch(ledModeState)
  {
    case 0:
    blankAllLeds();
    if(singleShotFlag == 1)
    {
      Serial.println("case 0");
      blankAllLeds();
      singleShotFlag = 0;
      delay(100);
    }
    break;

    case 1:
    ledFuncs_cylon();
//    showYellow();
//    if(singleShotFlag == 1)
//    {
//      showYellow();
//      Serial.println("case 1");
//      singleShotFlag = 0;
//    }
//    Serial.println("case 1");
    break;

    case 2:
    showBlue();
    if(singleShotFlag == 1)
    {
      showBlue();
      Serial.println("case 2");
      singleShotFlag = 0;
    }
//    Serial.println("case 2");
    break;

    case 3:
    ledFuncs_candle();
//    Serial.println("case 3");
    break;

    case 4:
//    Serial.println("case 4");
    rainbows();
    break;

    case 5:
    ledFuncs_waves();
    if(singleShotFlag == 1)
    {
      Serial.println("case 5");
      Serial.print("brightness: ");
      Serial.println(ledFuncs_getBrightness());
//      ledFuncs_setBrightness(getEepromBright());
      singleShotFlag = 0;
    }
//    Serial.println("case 5");
    break;
    
    case 6:
    blankAllLeds();
    if(singleShotFlag == 1)
    {
//      showWhite();
      Serial.println("case 6");
      singleShotFlag = 0;
    }
//    Serial.println("case 8");
    break;
    
    default:
    int i = 0;
    if(singleShotFlag == 1)
    {
//      showWhite();
      Serial.println("case default");
      singleShotFlag = 0;
    }
//    showRed();
  }
}
