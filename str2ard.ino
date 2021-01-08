/**************************************************************************

   Copyright (c) 2021 Peter Demmer
   Permission is hereby granted, free of charge, to anyone
   obtaining a copy of this document and accompanying files,
   to do whatever they want with them without any restriction,
   including, but not limited to, copying, modification and redistribution.
   NO WARRANTY OF ANY KIND IS PROVIDED.

   Arduino Wire Slave for Raspberry Pi Master
   String is sent from RPi to Arduino, response is sent back.
   I2C Wiring:
   RPi    Arduino
    9  GND  GND
    3  SDA  A4
    5  SCL  A5
   Note: I2C bus runs at 3.3V, Arduino can handle that, but
   DON'T USE pull-up resistors at the Arduino side !!!

**************************************************************************/


// Include the Wire library for I2C
#include <Wire.h>   // Uses {A4,A5} for {SDA,SCL}


#define NODE 11
#define LEN_DEBUG 64
#define LEN_MSG 32
#define WRREQ 0xFE
#define RDREQ 0xFD
char debug[LEN_DEBUG];
char rxmsg[LEN_MSG];
char txmsg[LEN_MSG];


void setup() {
  Serial.begin(9600);
  sprintf(debug, "-id=%02d, Wire slave", NODE);
  Serial.println(debug);
  
  // Join I2C bus as slave with address 11
  Wire.begin(NODE);
  
  // Call recvEvent when data received/written                
  Wire.onReceive(recvEvent);
  // Call sendEvent when data requested/read
  Wire.onRequest(sendEvent);
}


// Function that executes whenever data is received from Wire master
// This is the follow-up to the master's: SMBus.write_i2c_block_data(11, 0xFE, rxmsg)
void recvEvent(int howMany) {
  uint8_t incount = 0;
  uint8_t rxcount = 0;
  uint8_t msgtype = 0;
  while (Wire.available()) { 
    char c = Wire.read(); // receive byte as a character
    if (incount++ == 0) {   // first character is the message type
      msgtype = c;
    } else {
        rxmsg[rxcount++] = c;
    }
  }
  switch (msgtype) {
    case WRREQ: 
      rxmsg[rxcount] = 0;
      sprintf(debug, "rcvd: '%s' (%d)", rxmsg, strlen(rxmsg));
      Serial.println(debug);
      // RDREQ handled by sendEvent()
  }
}


// Function that executes whenever data is requested by Wire master
// This is the reply to the master's: SMBus.read_i2c_block_data(11, 0xFD, strlen(txmsg))
void sendEvent() {
  if (strlen(txmsg)) {
    uint8_t sent = Wire.write(txmsg);
    sprintf(debug, "sent: '%s' (%d)", txmsg, sent);
    Serial.println(debug);
    rxmsg[0] = 0;
    txmsg[0] = 0;
  }
}  


void loop() {
  // process received Wire message:
  if (strlen(rxmsg)) {
    // sample string processing: echo
    sprintf(txmsg, "%s", rxmsg);
    rxmsg[0] = 0;
  }
  delay(100);
}

