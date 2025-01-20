/*
   a18.lipocharger_4616 step-down, INA3221 current sensor, TP4056 LiPo-charger, 2004A LCD display

   Ref.: https://github.com/RobTillaart/INA3221_RT

   Board: Arduino Pro Mini
   Processor: Atmega328P (3.3V 8 MHz), but supplied  by 5V
   Port:  /dev/ttyUSB0 (Arduino Mini Pro)
   Programmer: AVRISP mk II
   Serial speed: 57600

  Sketch uses X bytes (X%) of program storage space. Maximum is 32256 bytes.
  Global variables use X bytes (X%) of dynamic memory, leaving X bytes for local variables. Maximum is 2048 bytes.

  Wiring dis2004A - Arduino:
    fnc - col- pin
    GND - BK - GND
    VCC - YE - VCC
    SDA - BN - A4
    SCL - GY - A5
  Wiring INA3221 - Arduino:
    fnc - col- pin
    VS  - YE - VCC
    GND - BK - GND
    SCL - GY - A5
    SDA - BN - A4
*/



#include <Arduino.h>
#include <Wire.h>


#include "myCli.h"
#include "myWire.h"
#include "myLiquidCrystal_I2C.h"
#include "myINA3221.h"


void setup(void) {
  cliSetup();

  wireSetup();

  lcdSetup();

  ina3221setup();
}


void loop(void) {
  cliLoop();

  ina3221loop();

  const uint32_t period = 1000L;  
  uint32_t wait = millis() % period;
  delay(period - wait);
}
