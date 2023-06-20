// myDisplay.cpp


#include "Arduino.h"
#include <SPI.h>
#include "myDisplay.h"
#include <Fonts/FreeSerifBold12pt7b.h>
#include <Fonts/FreeSerifBold18pt7b.h>
#include <Fonts/FreeSerifBold24pt7b.h>
#include "RTClib.h"


#include "myCli.h"
#include "myButton.h"
#include "myRTC.h"


Waveshare_ILI9486 tft;


// TO DO: support several screens with each a different layout
uint8_t kScreen = 1;          // initial screen
uint8_t brightness = 255;     // display brightness setting
uint16_t a15sum = 1023;       // used for measurement value filtering
uint16_t oldA15sum = a15sum;




void disSetup(void) {
  tft.begin();
  tft.setRotation(ROTATION);
  tft.setTextSize(2);
  tft.setScreenBrightness(brightness);

  if (lDeb > 0) {
    //uint16_t ID = tft.readID();
    uint16_t ID = 0x9486;
    sprintf(cliBuf, "sd:%d\tID=0x%X width(X,L->R)=%dp height(Y,U->D)=%dp", __LINE__, ID, WID, HEI);
    Serial.println(cliBuf);
  }

  if (lDeb > 2) {
    disShowCoordinates();
    delay(10000);
    tft.fillScreen(TFT_BLACK);
  }

  disClockSetup();
}




void disLoop(void) {
  disBrightness();

  disLoopA();
}




// Function: CLI control: show brightness, set screen number
uint8_t disCli(char* rxmsg, char* txmsg) {
  if (!strncmp(rxmsg, "dibr", 4)) {   // usage: "dibr;"
    if (rxmsg[4] == 0) {
      snprintf(cliBuf, sizeof(cliBuf), "dibr=%d;\n", brightness);
      Serial.print(cliBuf);
      return 0;
    }
  } else if (!strncmp(rxmsg, "disc", 4)) {   // set screen number
    if (rxmsg[4] == 0) {   // usage: "disc;", "disc=2;"
      snprintf(cliBuf, sizeof(cliBuf), "disc=%d;\n", kScreen);
      Serial.print(cliBuf);
      return 0;
    }
    if (rxmsg[4] != '=') {
      snprintf(cliBuf, sizeof(cliBuf), "E: rxcmd='%s'\n", rxmsg);
      Serial.print(cliBuf);
      return 1;
    } else {
      if (rxmsg[5] == '0') {
        kScreen = 0;
      } else if (rxmsg[5] == '1') {
        kScreen = 1;
        disClockSetup();
        btnSetup();
      } else if (rxmsg[5] == '2') {
        kScreen = 2;
        disClockSetup();
        btnSetup();
      } else {
        snprintf(cliBuf, sizeof(cliBuf), "disc:%d\tunknown screen='%c'\n", __LINE__, rxmsg[5]);
        Serial.print(cliBuf);
        return 1;
      }
      snprintf(cliBuf, sizeof(cliBuf), "disc:%d\tscreen=%d\n", __LINE__, kScreen);
      Serial.print(cliBuf);
      return 0;
    }
  }
}


void disClockSetup(void) {
  initial = true;
  
  disAnalogClockSetup();
}


// Control display brightness from ambient light:
void  disBrightness(void) {
  // An LDR measures ambient brightness and sets display brightness accordingly
  // Filter out LDR measurement noise by averaging e.g. the last 16 measurements:
  uint8_t nMeas = 32;   // maximum value: 64 == max(uint16_t) / 1023
  a15sum *= (nMeas - 1);
  a15sum += analogRead(A15);
  a15sum /= nMeas;

  uint16_t a15 = a15sum / 2;   // 50% of analog voltage => max brightness
  if (a15 > 255) a15 = 255;
  if (a15 < 1) a15 = 1;
  brightness = a15;
  tft.setScreenBrightness(brightness);
  if (lDeb > 2) {
    snprintf(cliBuf, sizeof(cliBuf), "dibr:%d:\ta15sum=%d->%d dibr=%d\n", __LINE__, oldA15sum, a15sum, brightness);
    Serial.print(cliBuf);
  }
  if (lDeb > 1 && a15sum != oldA15sum) {
    snprintf(cliBuf, sizeof(cliBuf), "dibr:%d:\ta15sum=%d->%d dibr=%d\n", __LINE__, oldA15sum, a15sum, brightness);
    Serial.print(cliBuf);
  }
  oldA15sum = a15sum;
}


// Display analog clock
// Analog clock code originally from: https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/blob/master/examples/TFT_eSPI/TFT_Clock/TFT_Clock.ino
// I enhanced the dials and made it a bit more parametric.
uint16_t radius;
uint16_t ccX;
uint16_t ccY;
boolean initial = true;

static uint8_t conv2d(const char *p); // Forward declaration needed for IDE 1.6.x
float sx = 0, sy = 1, mx = 1, mx1 = 1, mx2 = 1, my = 0, my1 = 0, my2 = 0, hx = -1, hx1 = -1, hx2 = -1, hy = 0, hy1 = 0, hy2 = 0; // Saved H, M, S x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = ccX, osy = ccY, omx = ccX, omx1 = ccX, omx2 = ccX, omy = ccY, omy1 = ccY, omy2 = ccY, ohx = ccX, ohx1 = ccX, ohx2 = ccX, ohy = ccY, ohy1 = ccY, ohy2 = ccY; // Saved H, M, S x & y coords
uint16_t xx0 = 0, xx1 = 0, yy0 = 0, yy1 = 0;
uint32_t targetTime = 0;                    // for next 1 second timeout
uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6);   // HH, MM, SS from compile time

DateTime dtBefore = DateTime(__DATE__, __TIME__);   // == compile time; if RTC time is before that, RTC time is set to compile time

// TO DO: replace by sscanf()/sprintf()
static uint8_t conv2d(const char *p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}


// Setup analog clock with circles, stripes and dots:
void disAnalogClockSetup(void) {
  if (kScreen == 1) {
    radius = HEI / 2;
    ccX = WID / 2;
    ccY = HEI / 2;
  } else if (kScreen == 2 ) {
    radius = WID / 4;
    ccX = WID / 2;
    ccY = HEI * 3 / 8;
  }

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);  // Adding a background colour erases previous text automatically

  // Draw clock face
  float cfro, cfri;          // clock face radius outside, inside
  cfro = .995, cfri = .95;
  tft.fillCircle(ccX, ccY, radius * cfro, TFT_GREY);
  tft.fillCircle(ccX, ccY, radius * cfri, TFT_BLACK);

  // Draw 12 red lines
  float hlo, hli;
  hlo = .99, hli = .90;
  for (uint16_t deg = 0; deg < 360; deg += 30) {
    sx = sin(deg * 0.0174532925);
    sy = -cos(deg * 0.0174532925);
    xx0 = sx * radius * hlo + ccX;
    yy0 = sy * radius * hlo + ccY;
    xx1 = sx * radius * hli + ccX;
    yy1 = sy * radius * hli + ccY;
    tft.drawLine(xx0, yy0, xx1, yy1, TFT_RED);
  }

  // Draw 60 white dots
  for (uint16_t deg = 0; deg < 360; deg += 6) {
    sx = sin(deg * 0.0174532925);
    sy = -cos(deg * 0.0174532925);
    xx0 = sx * radius * .85 + ccX;
    yy0 = sy * radius * .85 + ccY;
    xx0 = sx * radius * .90 + ccX;
    yy0 = sy * radius * .90 + ccY;
    // Draw minute markers
    tft.drawPixel(xx0, yy0, TFT_WHITE);
  }

  tft.fillCircle(ccX, ccY + 1, radius / 60, TFT_WHITE);

  // draw 12-o'clock marker
  uint16_t dx = 5;
  tft.fillRect(WID / 2 - dx, ccY - radius, 2 * dx, radius / 6, TFT_GREY);
}


// adjust clock dials, hour+minute hands move once per minute, second hand supported:
void disLoopA() {
  int16_t x1, y1;
  uint16_t w1, h1;

  DateTime dtNow = rtc.now();

  if (dtNow != dtBefore) {   // only repaint minutes/hours when time has changed
    dtBefore = dtNow;
    hh = dtNow.hour();
    mm = dtNow.minute();
    ss = dtNow.second();
    uint8_t dd = dtNow.day();
    uint8_t wd = dtNow.dayOfTheWeek();

    // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = ss * 6;                        // 0-59 -> 0-354
    //mdeg = mm * 6  + sdeg * 0.01666667;   // 0-59 -> 0-360 - includes seconds
    mdeg = mm * 6;                        // 0-59 -> 0-360 - seconds not used here since the minute hand is only set when seconds == 0
    hdeg = hh * 30 + mdeg * 0.0833333;    // 0-11 -> 0-360 - includes minutes

    uint16_t thm = 3, thh = 6;               // minute hand thickness, hour hand thickness in degrees / 2, second hand is a single line
    // dial consists of 2 triangles
    hx  = sin((hdeg)        * 0.0174532925);
    hx1 = sin((hdeg - thh)  * 0.0174532925);
    hx2 = sin((hdeg + thh)  * 0.0174532925);
    hy  = -cos((hdeg)       * 0.0174532925);
    hy1 = -cos((hdeg - thh) * 0.0174532925);
    hy2 = -cos((hdeg + thh) * 0.0174532925);

    mx  = sin((mdeg)        * 0.0174532925);
    mx1 = sin((mdeg - thm)  * 0.0174532925);
    mx2 = sin((mdeg + thm)  * 0.0174532925);
    my  = -cos((mdeg)       * 0.0174532925);
    my1 = -cos((mdeg - thm) * 0.0174532925);
    my2 = -cos((mdeg + thm) * 0.0174532925);

    sx  = sin((sdeg)        * 0.0174532925);
    sy  = -cos((sdeg)       * 0.0174532925);

    float hhd = .55;                        // outer hour hand length
    float hhl = .50;                        // hour hand length
    float mhd = .75;                        // outer minute hand length
    float mhl = .70;                        // minute hand length
    if (ss == 0 || initial) {   // only refresh minutes and hours once every minute to prevent the display from flashing each second
      // Erase hour hand: overwrite with black
      tft.fillTriangle(ccX, ccY + 1, ohx1, ohy1, ohx2, ohy2, TFT_BLACK);
      tft.fillTriangle(ohx, ohy, ohx1, ohy1, ohx2, ohy2, TFT_BLACK);
      ohx  = hx  * radius * hhd + ccX + 1;
      ohy  = hy  * radius * hhd + ccY + 1;
      ohx1 = hx1 * radius * hhl + ccX + 1;
      ohy1 = hy1 * radius * hhl + ccY + 1;
      ohx2 = hx2 * radius * hhl + ccX + 1;
      ohy2 = hy2 * radius * hhl + ccY + 1;

      // Erase minute hand: overwrite with black
      tft.fillTriangle(ccX, ccY + 1, omx1, omy1, omx2, omy2, TFT_BLACK);
      tft.fillTriangle(omx, omy, omx1, omy1, omx2, omy2, TFT_BLACK);
      omx  = mx  * radius * mhd + ccX;
      omy  = my  * radius * mhd + ccY + 1;
      omx1 = mx1 * radius * mhl + ccX;
      omy1 = my1 * radius * mhl + ccY + 1;
      omx2 = mx2 * radius * mhl + ccX;
      omy2 = my2 * radius * mhl + ccY + 1;

      // Erase day of the month
      if (initial || (hh == 0 && mm == 0)) {
        tft.getTextBounds("30", WID / 2, ccY - radius * .55, &x1, &y1, &w1, &h1);   // "30" == widest day string
        tft.fillRect(x1 - w1 / 2, y1, w1 + 5, h1 + 1, TFT_BLACK);
      }
      initial = false;
    }

    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker

    // show hour digits; these can be overwritten by the minute and second hand:
    // "0" is omitted (and should be shown as "12" anyway) and can be filled with e.g. day of the month
    tft.setTextColor(TFT_GREY);
    tft.setTextSize(1);
    if (kScreen == 2) {
      tft.setFont(&FreeSerifBold12pt7b);
    } else if (kScreen == 1) {
      tft.setFont(&FreeSerifBold18pt7b);
    }
    // Erase old second hand
    tft.drawLine(osx, osy, ccX, ccY + 1, TFT_BLACK);

    const float hdl = .80;   // hour digit location radius
    for (uint16_t deg = 90; deg < 360; deg += 90) {
      uint16_t xx2 = ccX + sin(deg * 0.0174532925) * radius * hdl;
      uint16_t yy2 = ccY - cos(deg * 0.0174532925) * radius * hdl;
      tft.setCursor(xx2 - radius / 24, yy2 + radius / 16);
      tft.print(deg / 30);
    }

    const float shl = .75;   // second hand length
    osx = sx * radius * shl + ccX + 1;
    osy = sy * radius * shl + ccY + 1;

    // repaint minute hand:
    tft.fillTriangle(ccX, ccY + 1, omx1, omy1, omx2, omy2, TFT_WHITE);
    tft.fillTriangle(omx, omy, omx1, omy1, omx2, omy2, TFT_WHITE);

    // repaint hour hand:
    tft.fillTriangle(ccX, ccY + 1, ohx1, ohy1, ohx2, ohy2, TFT_WHITE);
    tft.fillTriangle(ohx, ohy, ohx1, ohy1, ohx2, ohy2, TFT_WHITE);

    // paint new second hand:
    tft.drawLine(osx, osy, ccX, ccY + 1, TFT_RED);

    // small red dot in the center:
    tft.fillCircle(ccX, ccY + 1, 4, TFT_RED);

    // Repaint day of the month:
    char dag[3];
    sprintf(dag, "%d", dd);
    tft.setFont(&FreeSerifBold24pt7b);
    tft.getTextBounds(dag, WID / 2, ccY - radius * .55, &x1, &y1, &w1, &h1);
    tft.setCursor(x1 - w1 / 2, y1 + h1);
    if (wd >= 1 && wd <= 5)   // Monday-Friday
      tft.setTextColor(color565(0x00, 0x7F, 0x00), TFT_BLACK);   // dark green
    else if (wd == 6)         // Saturday
      tft.setTextColor(color565(0x7F, 0x7F, 0xFF), TFT_BLACK);   // light blue
    else if (wd == 0)         // Sunday
      tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setFont(&FreeSerifBold24pt7b);
    tft.print(dag);
  }
}


void disShowCoordinates() {   // screen layout helper, TFT screen development only
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  for (uint16_t y2 = 0; y2 < HEI; y2 += HEI / 16) {
    for (uint16_t x2 = 0; x2 < WID; x2 += WID / 6) {
      tft.setCursor(x2, y2);
      sprintf(cliBuf, "{%d,%d}", x2, y2);
      tft.print(cliBuf);
    }
  }
}


// Convert 3*8-bit color to 16-bit color:
uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

// dim or brighten 16-bit color:
uint16_t color565mult(uint16_t color565, float multiplier) {
  if (multiplier < 0.) multiplier = 0.;
  uint16_t red   = color565 >> 8; red   &= 0xF8;
  uint16_t green = color565 >> 3; green &= 0xFC;
  uint16_t blue  = color565 << 3; blue  &= 0xF8;
  red   = (uint16_t) (red   * multiplier); if (red   > 255) red = 255;
  green = (uint16_t) (green * multiplier); if (green > 255) green = 255;
  blue  = (uint16_t) (blue  * multiplier); if (blue  > 255) blue = 255;

  return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
}
