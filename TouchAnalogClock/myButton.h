#pragma once

// myButton.h


#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Waveshare_ILI9486.h"


extern struct tButton aaBut[];
extern uint8_t nButton;   // total number of buttons on any screen
extern uint8_t nScreen;   // number of screens;
extern int8_t iScreen;    // screen iterator


// Button colours:
extern uint16_t colbei, colbeid, colred, colredd, colbrn, colbrnd, colblu, colblud, collbr, collbrd, collgn, collgnd; 


#define DIM 0.6     /* dimmed = not active, undimmed = active */
#define TH 4        /* button border thickness */


// TO DO: add bool bVisible and color colBackground
struct tButton {
  uint8_t screen;
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
  uint16_t colin;
  uint16_t colbo;
  String text;
  bool pressed;
  bool down;
  bool on;
  uint16_t timeout;   // maximum on time in seconds
  uint32_t endtime;   // endtime in millis
};


void btnSetup(void);
void btnLoop(void);
void btnAction(uint8_t, uint32_t);
void btnPaint(uint8_t);
