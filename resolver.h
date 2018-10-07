#ifndef RESOLVER_H
#define RESOLVER_H

#include "includes.h"

unsigned char* get_name(uint8_t*,uint8_t*, int*);
void formatToDNS(unsigned char*, char*);
void decode_header(DNS_HEADER*, uint16_t);
int something_is_in_error(DNS_HEADER*);
void run_program(unsigned char*, char*, char*);
uint8_t* query_dns_server(unsigned char*, char*, int);

#endif
