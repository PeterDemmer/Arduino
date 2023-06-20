// myRTC.cpp


#include "Arduino.h"
#include "Wire.h"
#include "RTClib.h"
#include "myRTC.h"
#include "myCli.h"


RTC_DS3231 rtc;


//Check if RTC is available
bool rtcSetup(void) {
  DateTime dtNow;
  DateTime dtCompiled = DateTime(__DATE__, __TIME__);   // Shows compile time of THIS file !

  if (! rtc.begin()) {
    sprintf(cliBuf, "sr:%d\tERROR: Couldn't find RTC\n", __LINE__);
    Serial.print(cliBuf);
    return false;
  } else {
    // clock now runs on localtime, CET or CEST = 1 or 2 hours ahead of UTC, and needs to be adjusted the last Sunday of March and October
    // TO DO: implement clock runs on UTC and TZ=Europe/Amsterdam
    if (lDeb > 0) {
      sprintf(cliBuf, "sr:%d\tRTC compiled=%s lt (%ld), OK\n", __LINE__, rtcFormat(dtCompiled, 1), dtCompiled.unixtime());
      Serial.print(cliBuf);
    }

    dtNow = rtc.now();
    if (lDeb > 0) {
      sprintf(cliBuf, "sr:%d\tRTC now=%s lt (%ld)\n", __LINE__, rtcFormat(dtNow, 1), dtNow.unixtime());
      Serial.print(cliBuf);
    }
  }

  if (dtNow < dtCompiled) {
    sprintf(cliBuf, "sr:%d\tRTC time is before compile time: updating RTC.\n", __LINE__);
    Serial.print(cliBuf);
    sprintf(cliBuf, "sr:%d\tCompiled=%s lt   ", __LINE__, rtcFormat(dtCompiled, 1));
    Serial.print(cliBuf);
    rtc.adjust(dtCompiled);
    delay(1000);
    dtNow = rtc.now();
    sprintf(cliBuf, "Now=%s lt\n", rtcFormat(dtNow, 1));
    Serial.print(cliBuf);
  }

  return true;
}


// Function: show and adjust RTC time;   usage: "ti;"   "ti=+12;"   "ti=-10;"
uint8_t rtcCli(char* rxmsg, char* txmsg) {
  DateTime dtNow;
  dtNow = rtc.now();

  if (rxmsg[2] != '=') {
    sprintf(cliBuf, "rc:%d\tNow=%s lt (%ld)\n", __LINE__, rtcFormat(dtNow, 1), dtNow.unixtime());
    Serial.print(cliBuf);
  } else {
    switch (rxmsg[3]) {
      case '+': break;
      case '-': break;
      default:
        snprintf(cliBuf, sizeof(cliBuf), "rc:%d\tE: time adjust failed: '%s'", __LINE__, &rxmsg[3]);
        Serial.println(cliBuf);
        return 1;
        break;
    }
    int8_t result = -2;
    int32_t adjust1;
    sprintf(cliBuf, "rc:%d\tadd %ld seconds to time\n", __LINE__, adjust1);
    result = sscanf(&rxmsg[4], "%ld", &adjust1);
    if (result < 1) {
      snprintf(cliBuf, sizeof(cliBuf), "rc:%d\tE: time adjust failed: '%s'", __LINE__, rxmsg);
      Serial.println(cliBuf);
    } else {
      snprintf(cliBuf, sizeof(cliBuf), "cd:%d\ttime adjust with %c%ld seconds", __LINE__, rxmsg[3], adjust1);
      sprintf(cliBuf, "rc:%d\tOld=%s lt (%ld)", __LINE__, rtcFormat(dtNow, 1), dtNow.unixtime());
      Serial.println(cliBuf);

      TimeSpan ts = TimeSpan(adjust1);
      DateTime dtNew;
      if (rxmsg[3] == '+')
        dtNew = rtc.now() + ts;
      else
        dtNew = rtc.now() - ts;
      rtc.adjust(dtNew);
      dtNow = rtc.now();
      sprintf(cliBuf, "rc:%d\tNew=%s lt (%ld)\n", __LINE__, rtcFormat(dtNow, 1), dtNow.unixtime());
      Serial.print(cliBuf);
    }
    delay(DEBUG_KEEPTIME);
  }
  return 0;
}


char *rtcFormat(DateTime now, uint8_t iFormat) {
  // NOTE: there is e.g. DateTime.toString("DDD DD MMM") which provides e.g. "Sun 02 Apr"
  static char RTCdate[40];

  if (lDeb > 2) {
    snprintf(cliBuf, sizeof(cliBuf), "rf:%d\tformat=%d", __LINE__, iFormat);
    Serial.println(cliBuf);
  }

  RTCdate[0] = 0;

  uint8_t mo, dd, wd, hh, mi, ss; // m, d, H, M, S variables
  uint16_t yy; // Year is 16 bit int
  yy = now.year();
  mo = now.month();
  dd = now.day();
  wd = now.dayOfTheWeek();
  hh = now.hour();
  mi = now.minute();
  ss = now.second();

  if (iFormat == 0) {  // NL locale version, provides e.g. "zon 2 apr"
    char* aWD[] = { (char*) "zo", (char*) "ma", (char*) "di", (char*) "wo", (char*) "do", (char*) "vr", (char*) "za" };
    char* aMO[] = { (char*) "jan", (char*) "feb", (char*) "mrt", (char*) "apr", (char*) "mei", (char*) "jun", (char*) "jul", (char*) "aug", (char*) "sep", (char*) "okt", (char*) "nov", (char*) "dec" };
    sprintf(RTCdate, "%s %d %s", aWD[wd], dd, aMO[mo - 1]);
  } else if (iFormat == 1) {
    sprintf(RTCdate, "%04d-%02d-%02d %02d:%02d:%02d", yy, mo, dd, hh, mi, ss);
  } else if (iFormat == 2) {
    sprintf(RTCdate, "%02d:%02d:%02d", hh, mi, ss);
  } else if (iFormat == 3) {
    sprintf(RTCdate, "%2d:%02d", hh, mi);
  }

  if (lDeb > 2) {
    snprintf(cliBuf, sizeof(cliBuf), "rf:%d\tRTCdate='%s'", __LINE__, RTCdate);
    Serial.println(cliBuf);
  }

  return RTCdate;
}
