#pragma once

// myINA219.h
// control INA219 current sensor


#include <Wire.h>
#include "INA219.h"   // Ref.: https://github.com/RobTillaart/INA3221_RT, Rob Tillaart version


extern INA219 ina219;


extern bool ina219setup(void);
