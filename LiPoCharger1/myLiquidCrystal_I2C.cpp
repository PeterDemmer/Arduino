// myLiquidCrystal_I2C.cpp

// handle INA219 current sensor


#include <Arduino.h>
#include "myCli.h"
#include "myLiquidCrystal_I2C.h"
#include "myINA219.h"


LiquidCrystal_I2C lcd(DISADDR, DISCHARS, DISLINES);


bool lcdSetup(void) {
  lcd.init();                      // initialize the lcd

  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Hello, world!");
  lcd.setCursor(6, 1);
  lcd.print("Arduino!");
  lcd.setCursor(3, 2);
  lcd.print("LCM I2C 2004");
  lcd.setCursor(4, 3);
  lcd.print("have fun !");
  delay(2000);
  lcdLine(2, "");
  lcdLine(3, "");

  return true;
}


void lcdLoop(void) {
  static double coulomb = 0.;
  static float fSecs = 0.;
  static uint16_t logline = 0;
#define LOGPERIOD 30

  coulomb += ina219.getCurrent() * (float) displayPeriod / 1000.;   // runs every second
  uint16_t iCurrent = round(ina219.getCurrent() * 1000.);   // round() can not be called 2ce in the same sprintf()
  uint16_t imAh = round(coulomb * 1000. / 3600.);

  if ((logline % LOGPERIOD) == 0) {
    snprintf(cliBuf, sizeof(cliBuf), "lcd%d\tI=%d mA\tC=%d mAh\t", __LINE__, iCurrent, imAh);
    Serial.print(cliBuf);
  }

  // TP4056 cycle is 100 ... 200 ms; take samples over 500 ms period
  uint16_t v0min = 1024;
  uint16_t v0max = 0;
  uint32_t v0sum = 0L;
  uint16_t v1min = 1024;
  uint16_t v1max = 0;
  uint32_t v1sum = 0L;
  uint16_t v6min = 1024;
  uint16_t v6max = 0;
  uint32_t v6sum = 0L;
  uint16_t v7min = 1024;
  uint16_t v7max = 0;
  uint32_t v7sum = 0L;
  uint32_t endTime = millis() + 500L;
  float count = 0;
  while (millis() < endTime) {
    uint16_t v0 = analogRead(A0);
    uint16_t v1 = analogRead(A1);
    uint16_t v6 = analogRead(A6);
    uint16_t v7 = analogRead(A7);
    
    if (v0min > v0) v0min = v0;
    if (v0max < v0) v0max = v0;
    if (v1min > v1) v1min = v1;
    if (v1max < v1) v1max = v1;
    if (v6min > v6) v6min = v6;
    if (v6max < v6) v6max = v6;
    if (v7min > v7) v7min = v7;
    if (v7max < v7) v7max = v7;

    v0sum += v0;
    v1sum += v1;
    v6sum += v6;
    v7sum += v7;

    count += 1.;
  }

  uint16_t iVm = round((float)v6sum * 5000. / count / 1024.);    // 5V full-scale, 10-bit ADC
  uint16_t iVp = round((float)v7sum * 5000. / count / 1024.);
  uint16_t iVn = iVp - iVm;

  if ((logline % LOGPERIOD) == 0) {
    snprintf(cliBuf, sizeof(cliBuf), "Vbat=%d,%d,%d mV\t", iVm, iVn, iVp);
    Serial.print(cliBuf);
  }

  // Convert from 0...1023 to 0..5000 mV
  v0min = (uint32_t) v0min * 5000 / 1024;
  v0max = (uint32_t) v0max * 5000 / 1024;
  v1min = (uint32_t) v1min * 5000 / 1024;
  v1max = (uint32_t) v1max * 5000 / 1024;
  v6min = (uint32_t) v6min * 5000 / 1024;
  v6max = (uint32_t) v6max * 5000 / 1024;
  v7min = (uint32_t) v7min * 5000 / 1024;
  v7max = (uint32_t) v7max * 5000 / 1024;
  uint16_t v0avg = round((float)v0sum * 5000. / count / 1024.);    // 5V full-scale, 10-bit ADC
  uint16_t v1avg = round((float)v1sum * 5000. / count / 1024.);
  uint16_t v6avg = round((float)v6sum * 5000. / count / 1024.);    // 5V full-scale, 10-bit ADC
  uint16_t v7avg = round((float)v7sum * 5000. / count / 1024.);
  if ((logline % LOGPERIOD) == 0) {
    snprintf(cliBuf, sizeof(cliBuf), "V0=%d..%d..%d mV\tV1=%d..%d..%d mV\t", v0min, v0avg, v0max, v1min, v1avg, v1max);
    Serial.print(cliBuf);
    snprintf(cliBuf, sizeof(cliBuf), "V6=%d..%d..%d mV\tV7=%d..%d..%d mV\t", v6min, v6avg, v6max, v7min, v7avg, v7max);
    Serial.println(cliBuf);
  }

  if (iCurrent > 1) fSecs += 1.;
  uint32_t secs = round(fSecs);
  sprintf(cliBuf, "%ld:%02ld   %d mV               ", secs / 60L, secs % 60L, iVn);
  lcdLine(0, cliBuf);

  snprintf(cliBuf, sizeof(cliBuf), "%d mA   %d mAh          ", iCurrent, imAh);
  lcdLine(1, cliBuf);

  logline++;
}


void lcdLine(const uint8_t line, const char* text) {
  char lcdLine[DISCHARS + 1];
  if (DISCHARS == 16) sprintf(lcdLine, "%-16.16s", text);   // truncate to linesize and pad with spaces
  if (DISCHARS == 20) sprintf(lcdLine, "%-20.20s", text);
  lcd.setCursor(0, line % DISLINES);
  lcd.print(lcdLine);
}
