#define MODE_BUTTON_PIN 4 // dev mirror is 3; analog input pin to use as a digital input, A3 on uno;
#define BR_UP_PIN 6// dev mirror is 15; will need 1uf on these, not 0.1uf as that only debounces for 1ms and we need at least 10 I'd say
#define BR_DN_PIN 8// dev mirror is 14
#define INT_PIN 2 //interrupt pin; only 2,3 allowed

#define TIMERDELAY            500

int getSingleShotFlag();
int getLedModeState();
int getSuspendFlag();

void setSingleShotFlag(int val);
void setLedModeState(int val);
void setSuspendFlag(int val);

void modeButtonTest(int initConditionTest);
void brightnessUPmodeButtonTest(int initConditionTest);
void brightnessDNmodeButtonTest(int initConditionTest);
void stateMachine();
