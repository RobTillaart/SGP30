//
//    FILE: unit_test_001.cpp
//  AUTHOR: Rob Tillaart
//    DATE: 2021-01-07
// PURPOSE: unit tests for the SET library
//          https://github.com/RobTillaart/SET
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

// supported assertions
// ----------------------------
// assertEqual(expected, actual);               // a == b
// assertNotEqual(unwanted, actual);            // a != b
// assertComparativeEquivalent(expected, actual);    // abs(a - b) == 0 or (!(a > b) && !(a < b))
// assertComparativeNotEquivalent(unwanted, actual); // abs(a - b) > 0  or ((a > b) || (a < b))
// assertLess(upperBound, actual);              // a < b
// assertMore(lowerBound, actual);              // a > b
// assertLessOrEqual(upperBound, actual);       // a <= b
// assertMoreOrEqual(lowerBound, actual);       // a >= b
// assertTrue(actual);
// assertFalse(actual);
// assertNull(actual);

// // special cases for floats
// assertEqualFloat(expected, actual, epsilon);    // fabs(a - b) <= epsilon
// assertNotEqualFloat(unwanted, actual, epsilon); // fabs(a - b) >= epsilon
// assertInfinity(actual);                         // isinf(a)
// assertNotInfinity(actual);                      // !isinf(a)
// assertNAN(arg);                                 // isnan(a)
// assertNotNAN(arg);                              // !isnan(a)

#include <ArduinoUnitTests.h>


#include "SGP30.h"



unittest_setup()
{
}

unittest_teardown()
{
}


unittest(test_constructor)
{
  fprintf(stderr, "VERSION: %s\n", SGP30_LIB_VERSION);
  SGP30 SGP;
  
  assertTrue(SGP.begin());
  assertTrue(SGP.isConnected());

  assertEqual(0x00, SGP.lastMeasurement());
  assertEqual(0x00, SGP.lastError());
}


unittest(test_constants)
{
  assertEqual(SGP30_OK, 0x00);
}


unittest(test_defaults_core)
{
  SGP30 SGP;

  assertEqual(0x00, SGP.getCO2());
  assertEqual(0x00, SGP.getTVOC());
  assertEqual(0x00, SGP.getH2_raw());
  assertEqual(0x00, SGP.getEthanol_raw());
  assertEqualFloat(0, SGP.getH2(), 0.001);
  assertEqualFloat(0, SGP.getEthanol(), 001);
}


unittest(test_sref_H2)
{
  SGP30 SGP;

  assertEqual(13119, SGP.getSREFH2());

  SGP.setSREFH2(0);
  assertEqual(0, SGP.getSREFH2());

  SGP.setSREFH2(10000);
  assertEqual(10000, SGP.getSREFH2());
  
  SGP.setSREFH2();
  assertEqual(13119, SGP.getSREFH2());
}


unittest(test_sref_Ethanol)
{
  SGP30 SGP;

  assertEqual(18472, SGP.getSREFEthanol());

  SGP.setSREFEthanol(0);
  assertEqual(0, SGP.getSREFEthanol());

  SGP.setSREFEthanol(10000);
  assertEqual(10000, SGP.getSREFEthanol());
  
  SGP.setSREFEthanol();
  assertEqual(18472, SGP.getSREFEthanol());
}


unittest_main()


// --------
