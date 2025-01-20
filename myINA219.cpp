// myINA219.cpp

// handle INA219 current sensor
// Ref.: https://www.best-microcontroller-projects.com/tp4056.html


#include <Arduino.h>
#include "myCli.h"
#include "myINA219.h"
#include "myLiquidCrystal_I2C.h"


INA219 ina219(0x40);   // Ref.: https://github.com/RobTillaart/INA3221_RT, Rob Tillaart version


bool ina219setup(void) {
  if (! ina219.begin()) {
    sprintf(cliBuf, "su%d\tINA219 sensor initiation failed", __LINE__);
    Serial.println(cliBuf);
    return false;
  }
  ina219.setBusADC(0x03);
  ina219.setShuntADC(0x03);
  ina219.setMaxCurrentShunt(3.2, 0.1);  // Ampere, Ohm

  sprintf(cliBuf, "in%d\tshunt=%d mOhm\n", __LINE__, round(ina219.getShunt() * 1000));
  Serial.println(cliBuf);

  return true;
};
