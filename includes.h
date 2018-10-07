#ifndef DUG_INCLUDES_H
#define DUG_INCLUDES_H

// structures
#include "dns_query_structures.h"
#include "cache.h"
#include "globals.h"

#include <stdio.h> 
#include <string.h>  //good for the meme set
#include <stdlib.h> 
#include <sys/socket.h>
#include <errno.h>
#include <sys/types.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <getopt.h>

// pretty colors
#define KRED   "\x1b[31m"
#define KYEL   "\x1b[33m"
#define KGREEN "\x1b[32m"
#define KBLUE  "\x1b[34m"
#define KRESET "\x1b[0m"


//port always 53
#define PORT 53
#define ROOT_SERVER "198.41.0.4"


#endif
