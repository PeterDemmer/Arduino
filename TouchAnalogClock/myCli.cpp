// myCli.cpp
// handle CLI COMMANDS


#include <Arduino.h>
#include "myCli.h"
#include "myDisplay.h"


char cliBuf[128];
char rxmsg[MSGLEN + 1];
char txmsg[MSGLEN + 1];
uint8_t lDeb = 0;




void cliSetup(void) {
  Serial.begin(115200);
  delay(1000);
  sprintf(cliBuf, "\n\nlc:%d\tArduino MEGA 2560 with Waveshare TFT 3.5\" TouchShield", __LINE__);
  Serial.println(cliBuf);
}




void cliLoop(void) {
  if (cliGetSerial(rxmsg)) {
    if (strlen(rxmsg) > 0) {
      cliSplit(rxmsg, txmsg);
    }
  }
}




// get CLI commands from Serial and execute them:
bool cliGetSerial(char *rxmsg) {
  char ch;
  int myI = 0;
  bool myStop = false, sereceived = false;

  // Read commands string from Serial
  while (Serial.available() && !myStop) {
    ch = Serial.read();
    if (ch == '\n') {
      myStop = true;
    } else {
      rxmsg[myI++] = ch;
    }
    if (myI >= MSGLEN)
      myStop = true;
    sereceived = true;
    delay(10);   // to allow for next char
  }
  rxmsg[myI] = 0;

  return sereceived;
}


// split Command Line in rxmsg (separated by ';') into single commands:
void cliSplit(char* rxmsg, char* txmsg) {
  txmsg[0] = 0;

  int myC = 0;
  char myCmd[MSGLEN + 1];
  for (uint8_t myI = 0; myI < strlen(rxmsg); myI++) {
    if (rxmsg[myI] == ';' || rxmsg[myI] == 0 || rxmsg[myI] == '\n') {
      myCmd[myC] = 0;
      myC = 0;
      if (myCmd[0] != 0) {
        cliExec(myCmd, txmsg);   // writes and serialprints txmsg
      }
    } else {
      myCmd[myC] = rxmsg[myI];
      myC++;
    }
  }

  rxmsg[0] = 0;
}


// Function: execute a single Command:
void cliExec(char *rxmsg, char* txmsg) {
  if (strstart(rxmsg, "de")) {   // de; de=0; de=1;
    cliDebug(rxmsg, txmsg);
  } else if (strstart(rxmsg, "di")) {   // dibr; disc; disc=1;
    disCli(rxmsg, txmsg);
  } else if (strstart(rxmsg, "do")) {   // do; do32=H; do48=L;
    doCli(rxmsg, txmsg);
  } else if (strstart(rxmsg, "ti")) {   // "ti;" "ti=+60;" "ti=-3600;"   TO DO: ti=y2023; ti=m12; ti=d23; ti=H17; ti=M35; ti=S00;
    rtcCli(rxmsg, txmsg);
  } else  {
    snprintf(cliBuf, sizeof(cliBuf), "E: '%s'", rxmsg);
    Serial.println(cliBuf);
  }
}


// Function: set debug level;   usage: de;  de=0;   de=1;
void cliDebug(char* rxmsg, char* txmsg) {
  if (rxmsg[2] != '=') {
    snprintf(cliBuf, sizeof(cliBuf), "cd:%d\tdebug=%d", __LINE__, lDeb);
  } else {
    int8_t result = -2;
    result = sscanf(&rxmsg[3], "%d", &lDeb);
    if (result < 1) {
      snprintf(cliBuf, sizeof(cliBuf), "cd:%d\tE: set debug failed: '%s'", __LINE__, &rxmsg[3]);
    } else {
      snprintf(cliBuf, sizeof(cliBuf), "cd:%d\tdebug=>%d", __LINE__, lDeb);
    }
  }

  Serial.println(cliBuf);
  delay(DEBUG_KEEPTIME);
}
