#pragma once

// myCli.h
// handle CLI COMMANDS


#include "Arduino.h"
#include "myDO.h"
#include "myRTC.h"


#define DEBUG_KEEPTIME 1000
#define strstart(string, s) (! strncmp((string), (s), strlen(s)))
#define MSGLEN 64


extern char cliBuf[128];
extern char rxmsg[MSGLEN + 1];
extern char txmsg[MSGLEN + 1];
extern uint8_t lDeb;


// handle CLI commands:
void cliSetup(void);
void cliLoop(void);
// read command line from Serial:
bool cliGetSerial(char *);
// split command line in rxmsg separated by ';' into single commands:
void cliSplit(char*, char*);
// execute a single CLI command:
void cliExec(char*, char*);
// read and set the debug level:
void cliDebug(char*, char*);   
