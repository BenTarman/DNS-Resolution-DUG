#ifndef GLOBALS_H
#define GLOBALS_H

#include "includes.h"

extern int isVerbose; //global variable for  verbose mode
extern int isDig; //longer "dig-like" printing is default
extern int isMattDaemon; // see if we want to run program as a daemon (default no)
extern int cacheResults; //cache the results (default not cache)

void debug(char*, ...);

void cacheprint(char* format, ...);

void digprint(char* format, ...) ;

#endif
