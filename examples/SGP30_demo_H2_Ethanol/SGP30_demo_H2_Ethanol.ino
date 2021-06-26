//
//    FILE: SGP30_demo_H2_Ethanol.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo SGP30
//    DATE: 2021-06-24
//     URL: https://github.com/RobTillaart/SGP30
//          https://www.adafruit.com/product/3709

// 20 samples per second RAW data


#include "SGP30.h"


SGP30 SGP;
uint8_t count = 0;
uint32_t lastTime = 0;


void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
    yield();
  };

  Serial.print(__FILE__);
  Serial.println(SGP30_LIB_VERSION);
  Serial.println();

  Serial.print("BEGIN:\t");
  Serial.println(SGP.begin());
  Serial.print("TEST:\t");
  Serial.println(SGP.measureTest());
}


void loop()
{
  if (SGP.readRaw())
  {
    if (count == 0)
    {
      Serial.println("\nH2 \tEthanol");
      count = 10;
    }
    Serial.print(SGP.getH2());
    Serial.print("\t");
    Serial.print(SGP.getEthanol());
    Serial.println();
    count--;
  }


  if (millis() - lastTime >= 50)
  {
    lastTime = millis();
    SGP.requestRaw();
  }

}


// -- END OF FILE --
