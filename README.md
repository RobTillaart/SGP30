
[![Arduino CI](https://github.com/RobTillaart/SGP30/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/SGP30/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/SGP30.svg?maxAge=3600)](https://github.com/RobTillaart/SGP30/releases)

# SGP30

Arduino library for SGP30 environment sensor

Warning: experimental, 0.1.0 is functional not complete.

## Description

The SVP30 from Sensirion is an environment sensor that measures H2 and Ethanol in the air. From these numbers an intern algorithm derives an CO2 equivalent and a TVOC measurement. 

The CO2 units are ppm, the TVOC units are ppb. Units for H2 and Ethanol are unknown. For larger concentrations the resolution drops, see datasheet.

The sensor can be read up to once per second (1 Hz).

The library supports 2 types of interfaces, a synchronous and an asynchronous interface. The sync interface is blocking for up to 40 milliseconds which was enough to trigger the implementation of an async interface. Note: the sync interface is implemented with the async interface.


#### multi sensors.

The sensor has a fixed I2C address so only one sensor per I2C bus can be used. If one needs more, one should use an I2C multiplexer or an MCU with multiple I2C buses or switch the VCC as a sort of ChipSelect signal.


## Interface

### Constructor

- **SGP30(TwoWire \*wire = &Wire)** Constructor with optional the Wire interface as parameter.
- **bool begin()** .
- **bool begin(sda, scl)** idem, for the ESP32 where one can choose the I2C pins.
- **bool isConnected()** checks if the address is visable on the I2C bus.


### Meta

- **bool getID()** reads the sensor ID into 12 bytes. (needs rework).
- **uint16_t getFeatureSet()** returns 0x0022, indicates that commands used in this library are supported.
- **bool measureTest()** verify the chip is working.


### Synchronous measurements

- **uint32_t lastMeasurement()** timestamp in millis of the last sync measurement made. This convenience function is useful to prevent reading the sensor too often.
- **bool measure(bool all = false)** if all == false, only the TVOC and CO2 are updated (slow due to blocking), if all == true, also the H2 and the Ethanol values are updated (even slower). Note the measurement is slow as there is an active blocking until the sensor is done. If the last measurement is less than a second ago, no measurement is made and the function returns false.


### A-synchronous measurements

With the async interface, the user should control that reads are at least one second apart. The user should also take care not to mix up different requests. See examples. 

- **void request()** sends a request to the sensor to read CO2 and TVOC. 
- **bool read()** returns true if the last request is more than 12 millis ago the CO2 and TVOC are read and updated. Otherwise false is returned.
- **void requestRaw()** sends a request to the sensor to read H2 and Ethanol.
- **bool readRaw()** returns true if the last request is more than 25 millis ago the H2 and Ethanol are read and updated. Otherwise false is returned.


### Get the data

The library caches the last read values, and these are the functions to access them. 

- **uint16_t getTVOC()** gets the TVOC concentration (ppb)
- **uint16_t getCO2()** gets the CO2 **equivalent** concentration (ppm)
- **uint16_t getH2()** gets the H2 concentration. Units unknown.
- **uint16_t getEthanol()** gets the Ethanol concnetration. Units unknown.


### Calibration

- **float setRelHumidity(float T, float RH)** sets the compensation for temperature and **relative** humidity. These values can be obtained e.g. from an SHT30, DHT22 or similar sensor.
- **void setAbsHumidity(float AbsoluteHumidity)** sets the compensation for **absolute** humidity. 


### Baseline functions

- TODO


### MISC

- **int lastError()** returns last error. (needs rework)



## Todo

No particular order.

- measure performance at different I2C speeds.
- get units H2 right
- get units Ethanol right
- redo **getID()**
- redo **lastError()**
- should **getFeatureSet()** become **bool checkFeatureSet()**?
- implement get Acquisition Baseline() functions.
- test test test ....
= extend unit tests
- improve documentation


## Operational

See examples

## Links

https://www.adafruit.com/product/3709

