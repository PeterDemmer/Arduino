/*
   Console with analog clock using Arduino Mega, 3.5" TouchScreen using digital outputs
   By: Peter Demmer

   Features:
   Real Time Clock (RTC) with analog display including day of month
   Buttons to control several relais that can control supply for e.g. Lamp, TV, Radio and Amplifier
   Buttons show text and change color when  pressed and set
   Button timeout to auto-reset when some time has passed
   For most functions CLI control possible using Serial
   Debug messages on Serial and debug level support for development and troubleshooting
   RTC time adjustable via CLI and automatically increased by compile time
   Automatic screen brightness control
   Modular code setup using a .h and a .cpp file and a Setup() and a Loop() per function
   Logical function names 
   Operational data contained in simple tables that can easily be adjusted
      
   
   Compile Settings:
   Tools->Board: "Arduino Mega or Mega 2560"
   Tools->Processor: "Arduino Mega or Mega 2560"
   Tools->Programmer: "AVRISP mkII"
   
   Sketch uses 57898 bytes (22%) of program storage space. Maximum is 253952 bytes.
   Global variables use 3096 bytes (37%) of dynamic memory, leaving 5096 bytes for local variables. Maximum is 8192 bytes.


   Ref.: https://www.tinytronics.nl/shop/nl/displays/tft/waveshare-3.5-inch-tft-display-shield-480*320-pixels-met-touchscreen-uno-compatible
   Ref.: https://www.waveshare.com/wiki/3.5inch_TFT_Touch_Shield
   Ref.: https://www.waveshare.com/w/upload/b/b0/XPT2046.pdf
   Ref.: https://www.waveshare.com/w/upload/7/78/ILI9486_Datasheet.pdf
   Ref.: https://www.waveshare.com/w/upload/7/71/3.5inch_TFT_Touch_Shield_Schematic.pdf
   Ref.: https://www.waveshare.com/w/upload/5/59/3.5inch_TFT_Touch_Shield_User_Manual_EN.pdf
   My reference: AM01.slaapkamerbediening_3501
   
   TO DO:
    - add SD support, e.g. for showing graphs on the buttons
    - add IR send e.g. to control Radio and Television
    - buttons disappear when, after some time, no buttons have been pressed
*/


#include <Arduino.h>
#include <SPI.h>


#include "myButton.h"
#include "myCli.h"
#include "myDO.h"
#include "myDisplay.h"
#include "myRTC.h"
#include "myTouch.h"
#include "myWire.h"



void setup() {
  lDeb = 1;

  cliSetup();

  SPI.begin();

  wireSetup();

  doSetup();

  disSetup();

  rtcSetup();

  btnSetup();

  touSetup();
}




void loop() {
  cliLoop();

  touLoop();

  btnLoop();
  
  disLoop();
  // fixes black line across left-top button written at first loop():
  static bool once = true;
  if (once) {
    btnPaint(0);
    once = false;
  }
    
  delay(100);
}
