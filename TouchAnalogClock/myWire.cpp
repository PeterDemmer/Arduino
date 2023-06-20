// myWire.cpp


#include "Arduino.h"
#include "Wire.h"
#include "myCli.h"
#include "myWire.h"


//TwoWire Wire;


wireType wt[] = {
  { 0x20, 0x27, "PCF8574 8 digital IO" },
  { 0x32, 0x39, "PCF8574A 8 digital IO" },
  { 0x56, 0x63, "AT24C32 EEPROM 32kB" },
  { 0x68, 0x68, "DS3231 RTC" },
};
uint8_t nWT = sizeof(wt) / sizeof(wireType);


void wireSetup(void) {
  Wire.begin();
  wireScan();
}


void wireScan(void) {
  byte error, address;
  uint8_t nDevices;

  snprintf(cliBuf, sizeof(cliBuf), "wsc:%d\tScanning I2C ...", __LINE__);
  Serial.println(cliBuf);

  nDevices = 0;
  for (address = 1; address < 127; address++ )  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)    {
      snprintf(cliBuf, sizeof(cliBuf), "wsc:%d\tI2C device type '%s' found at address 0x%02X", __LINE__, wireId2type(address).c_str(), address);
      Serial.println(cliBuf);
      nDevices++;
    } else if (error == 4)    {
      snprintf(cliBuf, sizeof(cliBuf), "wsc:%d\tUnknown error at address 0x%02X", __LINE__, address);
      Serial.println(cliBuf);
    }
  }

  if (nDevices == 0) {
    snprintf(cliBuf, sizeof(cliBuf), "wsc:%d\t... no devices found", __LINE__);
    Serial.println(cliBuf);
  }  else {
    snprintf(cliBuf, sizeof(cliBuf), "wsc:%d\t... %d devices found, done", __LINE__, nDevices);
    Serial.println(cliBuf);
  }
}


String wireId2type(byte address) {
  for (uint8_t iWT = 0; iWT < nWT; iWT++) {
    if (wt[iWT].startAddr <= address && address <= wt[iWT].endAddr) {
      return wt[iWT].wtype;
    }
  }
  return "?";
}
