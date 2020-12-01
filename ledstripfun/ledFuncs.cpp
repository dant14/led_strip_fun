#include "Arduino.h"
#include "ledFuncs.h"

#include "nvsFuncs.h"
#include "buttons.h"


#define FRAMES_PER_SECOND           60
#define COOLING                     55
#define SPARKING                    70

CRGBArray<NUM_LEDS> leds;

// Define the array of leds
//CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;
bool gReverseDirection = false;

unsigned long currentMillis;
unsigned long startMillis;
unsigned long endMillis;

void ledFuncs_brightnessIndicator(byte previousBrState, byte targetBrState);
byte ledFuncs_getLUTval(int idx);
int ledFuncs_getLUTidx(byte brightness);
void Fire2012WithPalette();
void pride();
void ledFuncs_waves();
void pacifica_loop();
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff);
void pacifica_add_whitecaps();
void pacifica_deepen_colors();

static const byte bright_lookUpTable[(BRIGHTNESS_LEVELS + 1)] = 
{      // led number; kinda increments in lots of 15, TBC
  30,  // 1
  45,  // 2
  60,  // 3
  75,  // 4
  90,  // 5
  105, // 6
  135, // 7
  195, // 8
  210, // 9
  240 //  10
};

#define SIZE_BRIGHTNESS_LUT (sizeof (bright_lookUpTable) / sizeof(byte))

void setupLeds()
{
  int brightness = getEepromBright();
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS)  // GRB ordering is assumed
  .setCorrection(TypicalSMD5050);
//  gPal = CRGBPalette16( CRGB::Black, CRGB::Green, CRGB::Purple,  CRGB::Orange);
//  chooseNextColorPalette(gTargetPalette);
  gPal = HeatColors_p;
  Serial.print(brightness);
  FastLED.setBrightness(ledFuncs_getLUTval(brightness));
  startMillis = millis();
  Serial.print("size of lut array: ");
  Serial.println(SIZE_BRIGHTNESS_LUT);
//  showWhite();
}


void fadeall() 
{ 
  for(int i = 0; i < NUM_LEDS; i++) 
  { 
    leds[i].nscale8(250);
  } 
}


void blankAllLeds()
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void ledFuncs_setBrightness(int val)
{
  FastLED.setBrightness(ledFuncs_getLUTval(val));
}

byte ledFuncs_getBrightness()
{
  return (byte)(FastLED.getBrightness());
}

byte ledFuncs_incrBrightByStep(byte prevVal)
{
  byte nextVal = prevVal;
  if((prevVal >= 0) && (prevVal <= BRIGHTNESS_LEVELS))
  {
    nextVal = prevVal + 1;
    ledFuncs_brightnessIndicator(prevVal, nextVal);
  }
  return nextVal;
}

byte ledFuncs_decBrightByStep(byte prevVal)
{
  byte nextVal = prevVal;
  if((prevVal >= 0) && (prevVal <= BRIGHTNESS_LEVELS))
  {
    nextVal = prevVal - 1;
    ledFuncs_brightnessIndicator(prevVal, nextVal);
  }
  return nextVal;
}

void ledFuncs_brightnessIndicator(byte previousBrState, byte targetBrState)
{
  blankAllLeds();
  FastLED.setTemperature(SodiumVapor);
  
  if(targetBrState > previousBrState)
  {
    for(byte i = (bright_lookUpTable[targetBrState] - 15); i <= bright_lookUpTable[targetBrState]; i++)
    {
      for(int x = 0; x <= (int)targetBrState*15; x++)
      {
        leds[x] = CRGB::White;
      }
      FastLED.setBrightness(i);
      Serial.print("brightnessssss: ");
      Serial.println(i);
      FastLED.show();
      FastLED.delay(5);
    }
  }
  else
  {
    for(byte i = (bright_lookUpTable[targetBrState] + 15); i >= bright_lookUpTable[targetBrState]; i--)
    {
      for(int x = 0; x <= (int)targetBrState*15; x++)
      {
//        Serial.print("led id: ");
//        Serial.println(x);
        leds[x] = CRGB::White;
      }
      FastLED.setBrightness(i);
      Serial.print("brightnessssss: ");
      Serial.println(i);
      FastLED.show();
      FastLED.delay(5);
    }
  }
//  delay(300);
  
}

int ledFuncs_getLUTidx(byte brightness)
{
  int ret = 999;// just an error value, brightness needs to be in 15 increments
  for(int i = 0; i < (int)SIZE_BRIGHTNESS_LUT; i++)
  {
    if(bright_lookUpTable[i] == brightness) ret = i;
  }
  if(ret != 999) return ret;
  else
  {
    Serial.println("invalid LUT brightness!!!!!!");
    return 9;// setting it to something sensible
  }
}

byte ledFuncs_getLUTval(int idx)
{
  if(idx >= 0 && idx < SIZE_BRIGHTNESS_LUT) return bright_lookUpTable[idx];
  else
  {
    Serial.println("invalid LUT index!!!!!!");
    return 240;
  }
}
////////////////////////////////#################################################################################################################################
void modeIndicator()
{
  int mode;
  int led1;
  int led2;
  int ledModeState = getLedModeState();
  mode = ledModeState - 1;//led array counts from 0, our mode is 1-6 cuz reasons

  // we have 5 modes and 10 leds to they will indicate which mode is activated by flashing 2 of them
  led1 = mode*2;
  led2 = led1+1;

  FastLED.setTemperature(SodiumVapor);
  
  for(int i = 0; i < 151; i++)
  {
    FastLED.setBrightness(i);
    leds[led1] = CRGB::Yellow;
    leds[led2] = CRGB::Yellow;
    FastLED.show();
    FastLED.delay(2);
  }
}


void showYellow()
{
  FastLED.setTemperature(Candle);
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Yellow;
  }
  FastLED.show();
}

void showRed()
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Red;
  }
  FastLED.show();
}

void showBlue()
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::DarkMagenta  ;
  }
  FastLED.show();
}

void showWhite()
{
  FastLED.setBrightness(255);
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::White;
  }
  FastLED.show();
}

void ledFuncs_candle()
{
//  Serial.println("CANDLE LOOP");
    unsigned int period = 50;
//  globalCtr++;
//  if(globalCtr == 1000)//needs to be revised
//  {
    currentMillis = millis();
    if(currentMillis - startMillis >= period)
    {
      FastLED.setTemperature(Candle);
      random16_add_entropy(random());
      Fire2012WithPalette(); // run simulation frame, using palette colors
    
      FastLED.show(); // display this frame
      startMillis = currentMillis;
//      FastLED.delay(1000 / FRAMES_PER_SECOND);
    }
//    globalCtr = 0;
//  }
}

void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < NUM_LEDS; i++)
  {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = NUM_LEDS - 1; k >= 2; k--)
  {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if (random8() < SPARKING)
  {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  // Step 4.  Map from heat cells to LED colors
  for (int j = 0; j < NUM_LEDS; j++)
  {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8(heat[j], 240);
    CRGB color = ColorFromPalette(gPal, colorindex);
    int pixelnumber;
    if (gReverseDirection)
    {
      pixelnumber = (NUM_LEDS - 1) - j;
    } else
    {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
}

void rainbows()
{
  FastLED.setTemperature(CarbonArc);
  pride();
  FastLED.show();
}

void pride() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}


CRGBPalette16 pacifica_palette_1 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 = 
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33, 
      0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };

void ledFuncs_waves()
{
//  EVERY_N_MILLISECONDS( 10 ) {
    FastLED.setTemperature(ClearBlueSky);
    pacifica_loop();
    FastLED.show();
//  }
}

void pacifica_loop()
{
  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011,10,13));
  sCIStart2 -= (deltams21 * beatsin88(777,8,11));
  sCIStart3 -= (deltams1 * beatsin88(501,5,7));
  sCIStart4 -= (deltams2 * beatsin88(257,4,6));

  // Clear out the LED array to a dim background blue-green
  fill_solid( leds, NUM_LEDS, CRGB( 2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}

// Add one layer of waves into the led array
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );
  
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i].blue = scale8( leds[i].blue,  145); 
    leds[i].green= scale8( leds[i].green, 200); 
    leds[i] |= CRGB( 2, 5, 7);
  }
}

void ledFuncs_cylon()
{
  static uint8_t hue;
  static uint8_t cylon_cntr;
  static uint8_t cylon_isReverse;

  if ((cylon_cntr < NUM_LEDS) && (cylon_isReverse == 0))
  {
    leds[cylon_cntr] = CHSV(hue++, 255, 255);
    FastLED.show();
    fadeall();
    cylon_cntr++;
    if(cylon_cntr == (NUM_LEDS-1))
    {
      cylon_isReverse = 1;
    }
    FastLED.delay(2);
  }
  else if ((cylon_cntr >= 0) && (cylon_isReverse == 1))
  {
    leds[cylon_cntr] = CHSV(hue++, 255, 255);
    FastLED.show();
    fadeall();
    cylon_cntr--;
    
    if(cylon_cntr == 0)
    {
      cylon_isReverse = 0;
    }
    FastLED.delay(2);
  }
}

