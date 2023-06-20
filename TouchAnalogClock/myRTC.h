#pragma once

// myRTC.h


#include "Arduino.h"
#include "RTClib.h"


extern RTC_DS3231 rtc;


bool    rtcSetup(void);
uint8_t rtcCli(char*, char*);
char*   rtcFormat(DateTime, uint8_t);
