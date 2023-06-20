// myDO.cpp


#include "Arduino.h"
#include "myRTC.h"
#include "myCli.h"
#include "myDO.h"


// Documentation in "myWiring.h"
// Digital Output ports:

struct sDO DO[] = {   // NOTE: texts must match tButton aaBut[*,7] !
  { 34, "Lamp" },  { 36, "Amp" },    { 38, "WLAN" },  { 40, "Bed" },   // 220V
  // TO DO: move TV to 48
  { 42, "_" },    { 44, "TV" },  { 46, "Radio" },  { 48, "-" },   // 12V
};
uint8_t nDO = sizeof(DO) / sizeof(DO[0]);


// Configure Digital Output ports
void doSetup(void) {
  for (uint8_t iDO = 0; iDO < nDO; iDO++) {
    pinMode(DO[iDO].nr, OUTPUT);
    digitalWrite(DO[iDO].nr, LOW);
  }
}


// Function: show and adjust Digital Outputs;   usage: "do;"   "do25;  "do27=H;"   "do07=L;"
uint8_t doCli(char* rxmsg, char* txmsg) {
  if (rxmsg[2] == 0) {
    for (uint8_t iDO = 0; iDO < nDO; iDO++) {
      snprintf(cliBuf, sizeof(cliBuf), "do%02d=%c,%s;", DO[iDO].nr, digitalRead(DO[iDO].nr) ? 'H' : 'L', DO[iDO].text.c_str());
      Serial.print(cliBuf);
      if (iDO % 4 == 3) Serial.println();
    }
    return 0;
  }

  if (rxmsg[2] < '0' || rxmsg[2] > '9' || rxmsg[3] < '0' || rxmsg[3] > '9') {
    snprintf(cliBuf, sizeof(cliBuf), "doc:%d\tE: Digital Output: wrong portnr '%s'", __LINE__, rxmsg);
    Serial.println(cliBuf);
    return 1;
  }

  int8_t result;
  int portnr;
  char mode;
  result = -2;
  result = sscanf(&rxmsg[2], "%02d", &portnr);
  if (result < 1) {
    snprintf(cliBuf, sizeof(cliBuf), "doc:%d\tE: Digital Output: set failed: '%s'", __LINE__, rxmsg);
    Serial.println(cliBuf);
    return 2;
  }

  if (!doIsValid(portnr)) {
    snprintf(cliBuf, sizeof(cliBuf), "doc:%d\tE: %d is not a Digital Output: '%s'", __LINE__, portnr, rxmsg);
    Serial.println(cliBuf);
    return 3;
  }

  String text = "?";
  uint8_t index = doPortToIndex(portnr);
  if (index != 0xFF) text = DO[index].text;
  if (rxmsg[4] != '=') {
    snprintf(cliBuf, sizeof(cliBuf), "do%02d=%c,'%s';\n", portnr, digitalRead(portnr) ? 'H' : 'L', text.c_str());
    Serial.print(cliBuf);
    return 0;
  } else {
    mode = rxmsg[5];
    mode = toupper(mode);
    if (mode != 'H' && mode != 'L') {
      snprintf(cliBuf, sizeof(cliBuf), "doc:%d\tE: Digital Output %d set '%c' failed: '%s'", __LINE__, portnr, mode, rxmsg);
      Serial.println(cliBuf);
      return 4;
    } else {
      if (mode == 'L') digitalWrite(portnr, LOW);
      if (mode == 'H') digitalWrite(portnr, HIGH);
      // TO DO: set the color of the corresponding button accordingly
      snprintf(cliBuf, sizeof(cliBuf), "do%02d=%c,'%s';\n", portnr, digitalRead(portnr) ? 'H' : 'L', text.c_str());
      Serial.print(cliBuf);
      return 0;
    }
  }
}


// check if the port is one of the digital output ports
bool doIsValid(uint8_t portnr) {
  for (int iDO = 0; iDO < nDO; iDO++) {
    if (portnr == DO[iDO].nr) {
      return true;
    }
  }
  return false;   // not found
}


// lookup DO index from portnumber
uint8_t doPortToIndex(int8_t portnr) {
  for (uint8_t iDO = 0; iDO < nDO; iDO++) {
    if (portnr == DO[iDO].nr) {
      return iDO;
    }
  }
  return 0xFF;   // not found
}


// look up DO index from text
uint8_t doNameToIndex(String text) {
  for (uint8_t iDO = 0; iDO < nDO; iDO++) {
    if (text ==  DO[iDO].text) {
      return iDO;
    }
  }
  return 0xFF;   // not found
}
