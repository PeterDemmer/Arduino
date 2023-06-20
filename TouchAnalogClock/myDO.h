#pragma once

// myDO.h


#include "Arduino.h"

struct  sDO { uint8_t nr; String text; };
extern struct sDO DO[];


void doSetup(void);
uint8_t doCli(char*, char*);
bool doIsValid(uint8_t);
uint8_t doPortToIndex(int8_t);
uint8_t doNameToIndex(String);
