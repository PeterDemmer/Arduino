#pragma once

// myDisplay.h

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Waveshare_ILI9486.h"


// TFT TouchScreen pinout:
#define LCD_CS 10
#define LCD_CS_0        digitalWrite(LCD_CS, LOW)
#define LCD_CS_1        digitalWrite(LCD_CS, HIGH)

#define LCD_BL 9

#define LCD_RST 8
#define LCD_RST_0       digitalWrite(LCD_RST, LOW)
#define LCD_RST_1       digitalWrite(LCD_RST, HIGH)

#define LCD_DC 7
#define LCD_DC_0        digitalWrite(LCD_DC, LOW)
#define LCD_DC_1        digitalWrite(LCD_DC, HIGH)


#define WID tft.LCD_HEIGHT   /* 480, tft.width(); screen width = 72 mm as seen with USB on right side (landscape) @Rotation=3 */
#define HEI tft.LCD_WIDTH    /* 320, tft.height(); screen height = 48 mm as seen with USB on right side (landscape) @Rotation=3 */

// TO DO: support portrait rotations 2 and 4
#define ROTATION 1   /* landscape: USB connector to the left, Digital I/O ports to the right */
//#define ROTATION 3   /* landscape: Digital I/O ports to the left, USB connector to the right */

  
// Assign human-readable names to some common 16-bit color values:
#define TFT_BLACK   0x0000
#define TFT_BLUE    0x001F
#define TFT_RED     0xF800
#define TFT_GREEN   0x07E0
#define TFT_MAGENTA 0xF81F
#define TFT_WHITE   0xFFFF
#define TFT_GREY    color565mult(TFT_WHITE, 0.7)


extern Waveshare_ILI9486 tft;   // ID=0x9486


extern boolean initial;
extern uint8_t kScreen;
extern uint8_t brightness;


void disSetup(void);
uint8_t disCli(char*, char*);
void disLoop(void);
void disLoopA(void);
void disShowCoordinates(void);
void disClockSetup(void);
void disAnalogClockSetup(void);
void  disBrightness(void);
uint16_t color565(uint8_t, uint8_t, uint8_t);
uint16_t color565mult(uint16_t, float);
