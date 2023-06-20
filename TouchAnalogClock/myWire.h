#pragma once

// myWire.h


#include "Arduino.h"
#include "Wire.h"


struct wireType { uint8_t startAddr; uint8_t endAddr; String wtype; };
extern struct wireType wt[];


void wireSetup(void);
void wireScan(void);
String wireId2type(byte address);
