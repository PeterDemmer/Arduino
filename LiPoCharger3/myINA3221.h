#pragma once

// myINA3221.h
// control INA3221 current sensor


#include <Wire.h>
#include "INA3221.h"   // versie INA3221/INA3221.h


extern bool ina3221setup(void);
extern void ina3221loop(void);
extern void ina3221display(void);
