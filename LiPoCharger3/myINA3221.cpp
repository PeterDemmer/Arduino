// myINA3221.cpp

// handle INA3221 current sensor


#include <Arduino.h>
#include "myCli.h"
#include "myLiquidCrystal_I2C.h"
#include "myINA3221.h"


INA3221 ina3221(0x40);   // versie INA3221_RT/INA3221.h


bool ina3221setup(void) {
  if (! ina3221.begin()) {
    sprintf(cliBuf, "su%d\tError: device not found", __LINE__);
    Serial.println(cliBuf);
    return false;
  }

  Serial.print("DieID: \t");
  Serial.println(ina3221.getDieID(), HEX);
  Serial.print("ManID: \t");
  Serial.println(ina3221.getManufacturerID(), HEX);
  Serial.print(" Conf: \t");
  Serial.println(ina3221.getConfiguration(), HEX);

  ina3221.setShuntR(0, 0.100);
  ina3221.setShuntR(1, 0.100);
  ina3221.setShuntR(2, 0.100);

  Serial.println("\nCHAN\tCRITIC\tWARNING");
  for (int ch = 0; ch < 3; ch++)  {
    Serial.print(ch);
    Serial.print("\t0x");
    Serial.print(ina3221.getCriticalAlert(ch), HEX);
    Serial.print("\t0x");
    Serial.print(ina3221.getWarningAlert(ch), HEX);
    Serial.println();
  }

  Serial.println("\nShunt Voltage");
  Serial.print("  SVSUM:\t0x");
  Serial.println(ina3221.getShuntVoltageSum(), HEX);
  Serial.print("SVLIMIT:\t0x");
  Serial.println(ina3221.getShuntVoltageSumLimit(), HEX);

  Serial.println("\nMask/ Enable");
  Serial.print("M/E:\t0x");
  Serial.println(ina3221.getMaskEnable(), HEX);

  Serial.println("\nPower Limit");
  Serial.print("UPPER:\t0x");
  Serial.println(ina3221.getPowerUpperLimit(), HEX);
  Serial.print("LOWER:\t0x");
  Serial.println(ina3221.getPowerLowerLimit(), HEX);

  return true;
};


void ina3221loop(void) {
  ina3221display();
};


void ina3221display(void) {
  Serial.println("\nCHAN\tBUS\tSHUNT\t\tCURRENT\t\tPOWER");
  for (int ch = 0; ch < 3; ch++)  {
    Serial.print(ch);
    Serial.print("\t");
    Serial.print(ina3221.getBusVoltage(ch), 3);
    Serial.print(" V\t");
    Serial.print(ina3221.getShuntVoltage_mV(ch), 3);
    Serial.print(" mV\t");
    Serial.print(ina3221.getCurrent_mA(ch), 3);
    Serial.print(" mA\t");
    Serial.print(ina3221.getPower_mW(ch), 3);
    Serial.println(" mW");
    lcd.setCursor(0, ch);
    sprintf(cliBuf, "%4d mA             ", (int) ina3221.getCurrent_mA(ch));
    lcd.print(cliBuf);
  }
  
  lcd.setCursor(0, 3);
  sprintf(cliBuf, "%20.20s", "");
  lcd.print(cliBuf);
}
