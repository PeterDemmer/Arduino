// myLiquidCrystal_I2C.cpp

// handle INA219 current sensor


#include <Arduino.h>
#include "myCli.h"
#include "myLiquidCrystal_I2C.h"


LiquidCrystal_I2C lcd(0x27, 20, 4);


bool lcdSetup(void) {
  lcd.init();                      // initialize the lcd 
  
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, world!");
  lcd.setCursor(2,1);
  lcd.print("Ywrobot Arduino!");
  lcd.setCursor(0,2);
  lcd.print("Arduino LCM IIC 2004");
  lcd.setCursor(2,3);
  lcd.print("Power By Ec-yuan!");

  return true;
}

/*
void lcdLoop(void) {
  
}
*/
