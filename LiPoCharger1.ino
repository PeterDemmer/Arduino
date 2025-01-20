/*
   a18.lipocharger_4615 step-down, INA219 current sensor, TP4056 LiPo-charger, 2004A LCD display

   Board: Arduino Pro Mini
   Processor: Atmega328P (3.3V 8 MHz), but supplied  by 5V
   Port:  /dev/ttyUSB0 (Arduino Mini Pro)
   Programmer: AVRISP mk II
   Serial speed: 57600

  Sketch uses 12696 bytes (41%) of program storage space. Maximum is 30720 bytes.
  Global variables use 1337 bytes (65%) of dynamic memory, leaving 711 bytes for local variables. Maximum is 2048 bytes.

  Wiring dis2004A - Arduino:
    fnc - col- pin
    GND - BK - GND
    VCC - YE - VCC
    SDA - BN - A4
    SCL - GY - A5
  Wiring INA219 - Arduino:
    fnc - col- pin
    VCC - YE - VCC
    GND - BK - GND
    SCL - GY - A5
    SDA - BN - A4
    
  Wiring Voltages - Arduino
    fnc - col - pin
    Vb- RD - A6
    Vb+ RD - A7

  WiringTP4056 voltages - Arduino
    p7 - RD led - A1 - charging
    p6 - GN led - A0 - charged
  Note: 
    R3 charging resistor (to pin 2) increased from 1k2 (1A) to 2k7 (440 mA)

  Sketch uses 14298 bytes (46%) of program storage space. Maximum is 30720 bytes.
  Global variables use 1427 bytes (69%) of dynamic memory, leaving 621 bytes for local variables. Maximum is 2048 bytes.

*/



#include <Arduino.h>
#include <Wire.h>


#include "myCli.h"
#include "myWire.h"
#include "myLiquidCrystal_I2C.h"
#include "myINA219.h"


void setup(void) {
  cliSetup();

  wireSetup();

  lcdSetup();

  ina219setup();
}


void loop(void) {
  cliLoop();
  
  lcdLoop();

  // TO DO: make a function from this:
  uint32_t used = millis() % displayPeriod;
  delay (displayPeriod - used);
}
