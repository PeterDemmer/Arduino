// myWire.cpp

#include "Arduino.h"
#include "Wire.h"
#include "myCli.h"
#include "myWire.h"


bool wireSetup(void) {
  Wire.begin();   

  wireScan();

  return true;
}


void cliWire(char* rxmsg,  char* txmsg) {
  if (rxmsg[2] == 0) {   // "wi;"
    wireScan();
  }
}


// TO DO: store address ranges in an array
// TO DO: return a list of detected addresses
void wireScan(void) {
  byte error, address;
  int nDevices;
  snprintf(cliBuf, sizeof(cliBuf), "\n\nwisc%d\tScanning I2C ...", __LINE__);
  Serial.println(cliBuf);

  nDevices = 0;
  for (address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      snprintf(cliBuf, sizeof(cliBuf), "wisc%d\tI2C device found at address 0x%02X", __LINE__, address);
      Serial.print(cliBuf);
      if (address == 0x23 || address == 0x5C) {   // Note: ADDR must be connected to ground. DON'T use address 0x23 here, that can also be a display address !
        snprintf(cliBuf, sizeof(cliBuf), " - BH1750/GY-302 light sensor");
        Serial.println(cliBuf);
        delay(1000);
      } else if (0x20 <= address && address <= 0x27) {
        //i2cDisplay.address = address;
        snprintf(cliBuf, sizeof(cliBuf), " - 16*2 or 20*4 LCD display");
        Serial.println(cliBuf);
        delay(1000);
      } else if (0x40 <= address && address <= 0x4F) {
        //i2cColorSensor.address = address;
        snprintf(cliBuf, sizeof(cliBuf), " - INA219 or INA3221 current sensor");
        Serial.println(cliBuf);
        delay(1000);
      } else {
        Serial.println();
      }
      nDevices++;
    } else if (error == 4) {
      snprintf(cliBuf, sizeof(cliBuf), "wisc%d\tUnknown error at address 0x02X", __LINE__, address);
      Serial.println(cliBuf);
    }
  }

  if (nDevices == 0) {
    snprintf(cliBuf, sizeof(cliBuf), "wisc%d\tNo I2C devices found\n", __LINE__);
    Serial.println(cliBuf);
  } else {
    snprintf(cliBuf, sizeof(cliBuf), "wisc%d\t%d I2C devices found, done.\n", __LINE__, nDevices);
    Serial.println(cliBuf);
  }
}
