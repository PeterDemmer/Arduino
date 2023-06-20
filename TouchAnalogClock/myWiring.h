/*
   Wiring DS3231 RTC:
   GND   -  BK  -   GND
   +5V   -  YE  -   +5V
   D20 - SDA - BN - SDA
   D21 - SCL - GY - SCL

   Must match "myDO.cpp - Digital Output Ports
   
   Wiring Infrared, must match IR_SEND_PIN:
   D32 - IR LEDs for QE317 radio tuner and CS2HD2 TV tuner (HIGH => on)

   Wiring brightness control on A15:
   LDR is connected between +5V and A15
   10 kOhm logarithmic potentiometer is connected between A15 and 0V
   When there is more light, the conductance (= inverse of resistance) of the LDR increases, and the brightness increases
   
   Wiring MH 4-ch relay module for 220V (HIGH => on):
   GND - BK - GND
   +5V - YE - +5V
   D34 - BN - IN1
   D36 - BN - IN2
   D38 - BN - IN3
   D40 - BN - IN4
   Wiring MH 4-ch relay module for 12V (HIGH => on):
   D42 - BN - IN1
   D44 - BN - IN2
   D46 - BN - IN3
   D48 - BN - IN4

   Wiring SD-card
   SPI SCK  - D13
   SPI MISO - D12
   SPI MOSI - D11
   SPI CS   - D5

*/
