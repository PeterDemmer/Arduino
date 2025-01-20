#pragma once

// myLiquidCrystal_I2C.h
// control 2004A display


#include <Wire.h>
#include <LiquidCrystal_I2C.h>   // Ref.: https://github.com/johnrickman/LiquidCrystal_I2C
#define DISADDR 0x27
#define DISCHARS 20
#define DISLINES 4


#define displayPeriod 1000L


extern LiquidCrystal_I2C lcd;


extern bool lcdSetup(void);
extern void lcdLoop(void);
extern void lcdLine(const uint8_t,const char*);
