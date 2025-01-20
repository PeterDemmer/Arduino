#pragma once

// myLiquidCrystal_I2C.h
// control 2004A display


#include <Wire.h>
#include <LiquidCrystal_I2C.h>   // Ref.: https://github.com/johnrickman/LiquidCrystal_I2C


extern LiquidCrystal_I2C lcd;


extern bool lcdSetup(void);
//extern void lcdLoop(void);
