//
//    FILE: SGP30.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
//    DATE: 2021-06-24
// PURPOSE: SGP30 library for Arduino
//     URL: https://github.com/RobTillaart/SGP30
//          https://www.adafruit.com/product/3709
//
//  HISTORY:
//  0.1.0   2021-06-24 initial version
//


#include "SGP30.h"


/////////////////////////////////////////////////////
//
// CONSTRUCTOR
//
SGP30::SGP30(TwoWire *wire)
{
  _address  = 0x58;
  _wire     = wire;

  _tvoc     = 0;
  _co2      = 0;
  _h2       = 0;
  _ethanol  = 0;

  _lastTime = 0;
  _error    = SGP30_OK;
}


#if defined (ESP8266) || defined(ESP32)
bool SGP30::begin(uint8_t dataPin, uint8_t clockPin)
{
  _wire      = &Wire;
  if ((dataPin < 255) && (clockPin < 255))
  {
    _wire->begin(dataPin, clockPin);
  } else {
    _wire->begin();
  }
  if (! isConnected()) return false;
  return true;
}
#endif


bool SGP30::begin()
{
  _wire->begin();
  if (! isConnected()) return false;
  _init();
  return true;
}


bool SGP30::isConnected()
{
  _wire->beginTransmission(_address);
  return ( _wire->endTransmission() == 0);
}


// INITIAL VERSION - needs optimization
bool SGP30::getID()
{
  _command(0x3682);
  delay(1);
  if (_wire->requestFrom(_address, (uint8_t)9) == 9)
  {
    for (uint8_t i = 0, j = 0; i < 3; i++)
    {
      _id[j++] = _wire->read();
      _id[j++] = _wire->read();
      _wire->read();        // skip crc
    }
    return true;
  }
  return false;
}


// expect to return 0x0022
uint16_t SGP30::getFeatureSet()
{
  _command(0x202F);
  delay(10);
  uint16_t rv = 0;
  if (_wire->requestFrom(_address, (uint8_t)3) == 3)
  {
    rv = _wire->read() << 8;
    rv  += _wire->read();
    _wire->read();       // skip crc
  }
  return rv;
}


// WARNING: resets all devices on I2C bus.
void SGP30::GenericReset()
{
  _command(0x0006);
  _init();
};


bool SGP30::measureTest()
{
  uint16_t rv = 0;
  _command(0x2032);
  delay(220);  // Page 11
  if (_wire->requestFrom(_address, (uint8_t)3) == 3)
  {
    rv = _wire->read() << 8;
    rv  += _wire->read();
    _wire->read();       // skip crc
    return rv == 0xD400;
  }
  return false;
}


/////////////////////////////////////////////////////
//
// MEASUREMENT
//

// SYNCHRONUOUS MODUS
bool SGP30::measure(bool all)      // this is the workhorse
{
  // 1 second between measurements. P.09
  if (millis() - _lastTime < 1000) return false;
  _lastTime = millis();

  request();
  delay(12);
  read();

  if (not all) return true;

  requestRaw();
  delay(25);
  readRaw();
  return true;
}


// A-SYNCHRONUOUS INTERFACE
void SGP30::request()
{
  _lastRequest = millis();
  _command(0x2008);
}


bool SGP30::read()
{
  if (_lastRequest == 0) return false;
  if (millis() - _lastRequest < 13) return false;  // P11
  _lastRequest = 0;

  // TODO error handling
  // TODO CRC
  if (_wire->requestFrom(_address, (uint8_t)6) != 6)
  {
    return false;
  }
  _co2  =  _wire->read() << 8;
  _co2  += _wire->read();
           _wire->read();       // skip crc

  _tvoc =  _wire->read() << 8;
  _tvoc += _wire->read();
           _wire->read();       // skip crc
  return true;
}


void SGP30::requestRaw()
{
  _lastRequest = millis();
  _command(0x2050);
}

bool SGP30::readRaw()
{
  if (_lastRequest == 0) return false;
  if (millis() - _lastRequest < 26) return false;  // P11
  _lastRequest = 0;

  // TODO error handling
  // TODO CRC
  if (_wire->requestFrom(_address, (uint8_t)6) != 6)
  {
    return false;
  }
  _h2      =  _wire->read() << 8;
  _h2      += _wire->read();
              _wire->read();        // skip crc

  _ethanol =  _wire->read() << 8;
  _ethanol += _wire->read();
              _wire->read();        // skip crc
  return true;
}



/////////////////////////////////////////////////////
//
// CALIBRATION
//

// T  in °C
// RH == RelativeHumidity
float SGP30::setRelHumidity(float T, float RH)  // P10
{
  // page 10 datasheet
  // AH = AbsoluteHumidity
  // uint16_t AH = 216.7 * RH/100 * 6.117 * exp((17.62 * T)/(243.12 + T)) / (273.15 + T);
  float AbsoluteHumidity = (2.167 * 6.112) * RH ;
  AbsoluteHumidity *= exp((17.62 * T)/(243.12 + T));
  AbsoluteHumidity /=  (273.15 + T);

  setAbsHumidity(AbsoluteHumidity);
  return AbsoluteHumidity;
}


void SGP30::setAbsHumidity(float AbsoluteHumidity)
{
  uint16_t AH = AbsoluteHumidity;
  uint8_t tmp = (AbsoluteHumidity - AH) * 256;
  AH = (AH << 8) | tmp;

  _setCommand(0x2061, AH);   // P 11
}


/////////////////////////////////////////////////////
//
// PRIVATE
//
int SGP30::_command(uint16_t cmd)
{
  _wire->beginTransmission(_address);
  _wire->write(cmd >> 8);
  _wire->write(cmd & 0xFF);
  _error = _wire->endTransmission();
  return _error;
}


int SGP30::_setCommand(uint16_t cmd, uint16_t val)
{
  _wire->beginTransmission(_address);
  _wire->write(cmd >> 8);
  _wire->write(cmd & 0xFF);
  _wire->write(val >> 8);
  _wire->write(val & 0xFF);
  _wire->write(_CRC8(val));
  _error = _wire->endTransmission();
  return _error;
}


// for sending command - CRC lib.
// always 2 bytes
uint8_t SGP30::_CRC8(uint16_t data)
{
  uint8_t val[2];
  val[0] = data >> 8;
  val[1] = data & 0xFF;
  uint8_t crc = 0xFF;
  for(uint8_t i = 0; i < 2; i++) 
  {
    crc ^= val[i];
    for (uint8_t b = 8; b > 0; b--)
    {
      if (crc & 0x80)
        crc = (crc << 1) ^ 0x31;  // polynomial
      else
        crc <<= 1;
    }
  }
  return crc;
};


void SGP30::_init()
{
  _command(0x2003);
};



// -- END OF FILE --
