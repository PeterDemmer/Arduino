#pragma once

// myTouch.h


#define TP_CS 4
#define TP_CS_0    digitalWrite(TP_CS, LOW)
#define TP_CS_1    digitalWrite(TP_CS, HIGH)

#define TP_IRQ 3
#define GET_TP_IRQ    digitalRead(TP_IRQ)

#define TP_BUSY 6
#define GET_TP_BUSY    digitalRead(TP_BUSY)

#define SPI4W_Write_Byte(__DATA) SPI.transfer(__DATA)
#define SPI4W_Read_Byte(__DATA) SPI.transfer(__DATA)


#define MINPRESSURE 200
#define MAXPRESSURE 1000
// default: const int TS_LEFT = 907, TS_RT = 136, TS_TOP = 942, TS_BOT = 139; 
const int TS_LEFT = 960, TS_RT = 50, TS_TOP = 960, TS_BOT = 100;   // my TouchScreen's calibration


void touSetup(void);
void touLoop(void);
bool Touch_getXY(void);
