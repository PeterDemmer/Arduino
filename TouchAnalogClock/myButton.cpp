// myButton.cpp

#include "Adafruit_GFX.h"
#include <Fonts/FreeSerifBold12pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <Waveshare_ILI9486.h>
#include "myButton.h"
#include "myDO.h"
#include "myDisplay.h"
#include "myCli.h"



// Button colours:
uint16_t colbei = color565(0xF5, 0xF5, 0x7F);   // beige, lamp color
uint16_t colred = color565(0xFF, 0x00, 0x00);   // red, electric blanket color
uint16_t colgrn = color565(0x00, 0xBF, 0x00);   // green
uint16_t colgry = color565(0xBF, 0xBF, 0xBF);   // grey
uint16_t colbrn = color565(0xFF, 0x7F, 0x00);   // brown
uint16_t colblu = color565(0xBF, 0xBF, 0xFF);   // blue, amplifier color
uint16_t collbr = color565(0xC4, 0xA4, 0x84);   // lightbrown, radio color
uint16_t collgn = color565(0x70, 0xD0, 0x70);   // lightgreen, television color


// TO DO: more support for screen variable, now only Buttons of screen 1 are shown
struct tButton aaBut[] = {   // NOTE: texts must match DO[*,1] !
  // screen, x       y            w        h        inside   border   text       press, down,  on, timeout, endtime

  // NOT USED: screen 0: blank

  // screen 1: 4 * 6 grid, clock r=160 in the middle size 4 * 4
  // Left column:
  { 1, 0,            0,           WID / 5, HEI / 3, colbei,  colbei,  "Lamp",    false, false, false, 0,    0L },
  { 1, 0,            HEI * 1 / 3, WID / 6, HEI / 3, colgry,  colgry,  "Amp",     false, false, false, 0,    0L },
  { 1, 0,            HEI * 2 / 3, WID / 5, HEI / 3, colred,  colred,  "Bed",     false, false, false, 3600, 0L },
  // Right column:
  { 1, WID * 4 / 5,  0,           WID / 5, HEI / 3, colblu,  colblu,  "WLAN",    false, false, false, 0,    0L },
  { 1, WID * 5 / 6,  HEI * 1 / 3, WID / 6, HEI / 3, collgn,  collgn,  "TV",      false, false, false, 0,    0L },
  { 1, WID * 4 / 5,  HEI * 2 / 3, WID / 5, HEI / 3, collbr,  collbr,  "Radio",   false, false, false, 0,    0L },

  // NOT USED: screen 2: 4 * 5 grid with the clock r=120 top middle
  // Left column:
  { 2, 0,            0,           WID / 5, HEI / 4, colgry,  colgry,  "Amp",     false, false, false, 0,    0L },
  { 2, 0,            HEI * 1 / 4, WID / 5, HEI / 4, collbr,  collbr,  "Radio1",  false, false, false, 0,    0L },
  { 2, 0,            HEI * 2 / 4, WID / 5, HEI / 4, collbr,  collbr,  "Radio2",  false, false, false, 0,    0L },
  { 2, 0,            HEI * 3 / 4, WID / 5, HEI / 4, collbr,  collbr,  "Classic", false, false, false, 0,    0L },
  // Mid bottom row:
  { 2, WID * 1 / 5,  HEI * 3 / 4, WID / 5, HEI / 4, colblu,  colblu,  "WLAN",    false, false, false, 0,    0L },
  { 2, WID * 2 / 5,  HEI * 3 / 4, WID / 5, HEI / 4, colgrn,  colgrn,  "-",       false, false, false, 0,    0L },
  { 2, WID * 3 / 5,  HEI * 3 / 4, WID / 5, HEI / 4, colred,  colred,  "Bed",     false, false, false, 3600, 0L },
  // Right column:
  { 2, WID * 4 / 5,  0,           WID / 5, HEI / 4, colbei,  colbei,  "Lamp",    false, false, false, 0,    0L },
  { 2, WID * 4 / 5,  HEI * 1 / 4, WID / 5, HEI / 4, collgn,  collgn,  "TV 1",    false, false, false, 0,    0L },
  { 2, WID * 4 / 5,  HEI * 2 / 4, WID / 5, HEI / 4, collgn,  collgn,  "TV 2",    false, false, false, 0,    0L },
  { 2, WID * 4 / 5,  HEI * 3 / 4, WID / 5, HEI / 4, collgn,  collgn,  "TV 3",    false, false, false, 0,    0L },
};
uint8_t nButton = sizeof(aaBut) / sizeof(tButton);
uint8_t nScreen = 3;


// paint buttons for this screen:
void btnSetup(void) {
  for (uint8_t iBtn = 0; iBtn < nButton; iBtn++) {
    if (aaBut[iBtn].screen == kScreen) {
      btnPaint(iBtn);
    }
  }
}


void btnLoop(void) {
  // in loopTouch(), 0, or 1 button presses were detected: aaBut[jBtn].pressed = true;
  static uint32_t prbegin = 0L;

  // repaint each button that has changed:
  for (uint8_t jBtn = 0; jBtn < nButton; jBtn++) {
    if (aaBut[jBtn].screen == kScreen) {
      // pressed is the change, down is the status
      if (aaBut[jBtn].down != aaBut[jBtn].pressed) {
        // button press begin
        if (aaBut[jBtn].pressed && !aaBut[jBtn].down) {
          prbegin = millis();
          if (lDeb > 0) {
            sprintf(cliBuf, "bl:%d\t'%s' pressed", __LINE__, aaBut[jBtn].text.c_str());
            Serial.println(cliBuf);
          }
          aaBut[jBtn].down = aaBut[jBtn].pressed;


          btnPaint(jBtn);   // pressed => down
        }

        if (!aaBut[jBtn].pressed && aaBut[jBtn].down) {
          // button release
          uint32_t prduration = millis() - prbegin;
          prbegin = 0L;
          if (lDeb > 0) {
            sprintf(cliBuf, "bl:%d\t'%s' released after %ld ms", __LINE__, aaBut[jBtn].text.c_str(), prduration);
            Serial.println(cliBuf);
          }

          aaBut[jBtn].down = aaBut[jBtn].pressed;

          btnAction(jBtn, prduration);   // includes btnPaint()
        }
      }   // button changed since last time
    }   // button on this screen


    // timer: autorelease of a button after timeout
    if (aaBut[jBtn].on && aaBut[jBtn].timeout > 0) {
      if (millis() > aaBut[jBtn].endtime) {
        if (lDeb > 0) {
          sprintf(cliBuf, "bl:%d\tnow=%ld '%s' endtime has passed", __LINE__, millis(), aaBut[jBtn].text.c_str());
          Serial.println(cliBuf);
        }
        uint8_t index = doNameToIndex(aaBut[jBtn].text);
        if (index == 0xFF) {   // not found
          sprintf(cliBuf, "bl:%d\t'%s' no matching Digital Output found", __LINE__, aaBut[jBtn].text.c_str());
          Serial.println(cliBuf);
        } else {
          digitalWrite(DO[index].nr, LOW);
          aaBut[jBtn].on = false;
          btnPaint(jBtn);
        }
        if (lDeb > 0) {
          sprintf(cliBuf, "bl:%d\tdo%02d=%c;", __LINE__, DO[index].nr, digitalRead(DO[index].nr) ? 'H' : 'L');
          Serial.println(cliBuf);
        }
      }
    }
  }
}


void btnAction(uint8_t jBtn, uint32_t prduration) {   // in fact: toggle
  uint8_t index;
  // find Digital Output with this text, e.g. "LAMP" :
  index = doNameToIndex(aaBut[jBtn].text);
  if (index == 0xFF) {   // not found
    sprintf(cliBuf, "bl:%d\t'%s' no matching Digital Output found", __LINE__, aaBut[jBtn].text.c_str());
    Serial.println(cliBuf);
  } else {   // toggle
    if (aaBut[jBtn].on) {
      digitalWrite(DO[index].nr, LOW);
      aaBut[jBtn].on = false;
    } else {
      if (aaBut[jBtn].timeout > 0) {
        aaBut[jBtn].endtime = millis() + (uint32_t) aaBut[jBtn].timeout * 1000L;   // TO DO: check for overflow; ALTERNATIVE: use the RTC instead
        if (lDeb > 0) {
          sprintf(cliBuf, "bl:%d\t'%s': now=%ld endtime=%ld", __LINE__, aaBut[jBtn].text.c_str(), millis(), aaBut[jBtn].endtime);
          Serial.println(cliBuf);
        }
      }

      digitalWrite(DO[index].nr, HIGH);
      aaBut[jBtn].on = true;
    }
  }

  btnPaint(jBtn);

  if (lDeb > 0) {
    sprintf(cliBuf, "bl:%d\tdo%02d=%c;", __LINE__, DO[index].nr, digitalRead(DO[index].nr) ? 'H' : 'L');
    Serial.println(cliBuf);
  }
}


// control center and border color and text of a button when it is on/down/idle
void btnPaint(uint8_t iBtn) {
  uint16_t colin, colbo;

  if (aaBut[iBtn].on) {
    colin = color565mult(aaBut[iBtn].colin, 1.0);
    colbo = color565mult(aaBut[iBtn].colbo, DIM);
  } else if (aaBut[iBtn].down) {
    colin = color565mult(aaBut[iBtn].colin, 1.0);
    colbo = color565mult(aaBut[iBtn].colbo, 1.0);
  } else {
    colin = color565mult(aaBut[iBtn].colin, DIM);
    colbo = color565mult(aaBut[iBtn].colbo, DIM * DIM);
  }
  tft.fillRoundRect(aaBut[iBtn].x,      aaBut[iBtn].y,      aaBut[iBtn].w,          aaBut[iBtn].h,         10, colbo);   // border
  tft.fillRoundRect(aaBut[iBtn].x + TH, aaBut[iBtn].y + TH, aaBut[iBtn].w - 2 * TH, aaBut[iBtn].h - 2 * TH, 8, colin);   // inner

  // TO DO: support multiline button texts
  // TO DO: support graphs from an SD card
  tft.setFont(&FreeSerifBold12pt7b);
  tft.setTextSize(1);
  if (aaBut[iBtn].on) {
    tft.setTextColor(TFT_BLACK);
  } else {
    tft.setTextColor(TFT_WHITE);
  }
  int16_t x = WID / 2;
  int16_t y = HEI / 2;
  int16_t x1, y1;
  uint16_t w1, h1, dx, dy;

  // write the text on the button:
  String text = aaBut[iBtn].text;
  tft.getTextBounds(text, x, y, &x1, &y1, &w1, &h1);
  if (lDeb > 1) {
    sprintf(cliBuf, "bp: % d\t#=%d\tt='%s'\tx1=%d\ty1=%d\tw1=%d\th1=%d\n", __LINE__, iBtn, text.c_str(), x1, y1, w1, h1);
    Serial.print(cliBuf);
  }
  dx = aaBut[iBtn].x + aaBut[iBtn].w / 2 - w1 / 2;
  dy = aaBut[iBtn].y + aaBut[iBtn].h / 2 + h1 / 2;
  tft.setCursor(dx, dy);
  tft.println(text);
}
