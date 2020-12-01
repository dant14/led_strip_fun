#include "Arduino.h"
#include <EEPROM.h>

#define STATEADDR     1
#define BRIGHTADDR    2

byte getEepromState()
{
  byte val = EEPROM.read((uint16_t)STATEADDR);
  Serial.print("Read state from nvs: ");
  Serial.println(val);
  return val;
}

byte getEepromBright()
{
  byte val = EEPROM.read((uint16_t)BRIGHTADDR);
  Serial.print("Read brightness state level from nvs: ");
  Serial.println(val);
  return val;
}

void setEepromState(byte val)
{
  EEPROM.write((uint16_t)STATEADDR, val);
  Serial.print("Write state to nvs: ");
  Serial.println(val);
}

void setEepromBright(byte val)
{
  EEPROM.write((uint16_t)BRIGHTADDR, val);
  Serial.print("Write brightness state level to nvs: ");
  Serial.println(val);
}

