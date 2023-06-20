// myTouch.cpp


#include "Arduino.h"
#include "Adafruit_GFX.h"
#include <Waveshare_ILI9486.h>
#include "myTouch.h"
#include "myCli.h"
#include "myButton.h"
#include "myDisplay.h"


int pixel_x, pixel_y;     //Touch_getXY() updates global vars


void touSetup(void) {
#if (TOUCH_READ_FROM_INTERRUPT)
  attachInterrupt(
    PIN_TOUCH_INT,
    []
  {
    intTouch = true;
  },
  CHANGE);
#endif
}


void touLoop(void) {
  bool anyPressed = false;   // any Button pressed, not used yet
  bool elsePressed = false;   // Screen pressed but not on Button, not used yet

  for (uint8_t jBtn = 0; jBtn < nButton; jBtn++) aaBut[jBtn].pressed = false;

  TSPoint tp = tft.getPoint();
  if (ROTATION == 1) {
    pixel_x = map(tp.y, TS_RT, TS_LEFT, 0, tft.width());   // X,Y swapped !
    pixel_y = map(tp.x, TS_TOP, TS_BOT, 0, tft.height());
  } else { /* ROTATION == 3 */
    pixel_x = map(tp.y, TS_LEFT, TS_RT, 0, tft.width());   // X,Y swapped !
    pixel_y = map(tp.x, TS_BOT, TS_TOP, 0, tft.height());
  }


  bool pressed = (tp.z > MINPRESSURE && tp.z < MAXPRESSURE);

  if (pressed) {
    if (lDeb > 1) {
      sprintf(cliBuf, "tcl:%d\t{%d,%d,%d} -> {%d,%d}", __LINE__, tp.x, tp.y, tp.z, pixel_x, pixel_y);
      Serial.println(cliBuf);
    }
    uint16_t xs = pixel_x, ys = pixel_y;
    // show location pressed:
    if (lDeb > 0) tft.fillCircle(xs, ys, tp.z / 60, TFT_GREEN);

    for (uint8_t jBtn = 0; jBtn < nButton; jBtn++) {
      uint16_t xl = aaBut[jBtn].x;
      uint16_t xh = aaBut[jBtn].x + aaBut[jBtn].w;
      uint16_t yl = aaBut[jBtn].y;
      uint16_t yh = aaBut[jBtn].y + aaBut[jBtn].h;
      if (xs > xl && xs < xh && ys > yl && ys < yh) {
        aaBut[jBtn].pressed = true;   // will be shown on the screen in loopButton();
        anyPressed = true;   // ignored for now
        if (lDeb > 1) {
          sprintf(cliBuf, "tcl:%d\tmatches Button[%d] %d<x<%d %d<y<%d", __LINE__, jBtn, xl, xh, yl, yh);
          Serial.println(cliBuf);
        }
      }
    }
  }

  if (! anyPressed) elsePressed = true;   // not used yet
}
