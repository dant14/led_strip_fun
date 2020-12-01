#include <FastLED.h>
FASTLED_USING_NAMESPACE;

#define MAX_BRIGHTNESS      240
#define MIN_BRIGHTNESS      30
#define BRIGHTNESS_LEVELS   9  //same as number of leds for now (counting from 0 to 9)

#define MAX_MODES           6

#define NUM_LEDS            150
#define DATA_PIN            13 //dev mirror is 17

void setupLeds();
void blankAllLeds();
void ledFuncs_setBrightness(byte val);
byte ledFuncs_getBrightness();
byte ledFuncs_incrBrightByStep(byte prevVal);
byte ledFuncs_decBrightByStep(byte prevVal);
void modeIndicator();

void showYellow();
void showRed();
void showBlue();
void showWhite();
void ledFuncs_candle();
void rainbows();
void ledFuncs_waves();
void ledFuncs_cylon();
