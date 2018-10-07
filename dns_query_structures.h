// define basic DNS Header and Question structures
// as copied from the slides in class

#include <stdint.h>

#ifndef DUG_STRUCTURES_H
#define DUG_STRUCTURES_H

typedef struct
{
	uint16_t id;       // identification number
	uint8_t rd :1;     // recursion desired
	uint8_t tc :1;     // truncated message
	uint8_t aa :1;     // authoritive answer
	uint8_t opcode :4; // purpose of message
	uint8_t qr :1;     // query/response flag
	uint8_t rcode :4;  // response code
	uint8_t cd :1;     // checking disabled
	uint8_t ad :1;     // authenticated data
	uint8_t z :1;      // its z! reserved
	uint8_t ra :1;     // recursion available
	uint16_t q_count;  // number of question entries
	uint16_t ans_count; // number of answer entries
	uint16_t auth_count; // number of authority entries
	uint16_t add_count; // number of resource entries

} DNS_HEADER;


typedef struct
{
	uint16_t qtype;
	uint16_t qclass;

} QUESTION;


#pragma pack(push, 1)
typedef struct
{
	uint16_t type;
	uint16_t _class;
	uint32_t ttl;
	uint16_t data_len;

} R_DATA;
#pragma pack(pop)


struct RES_RECORD
{
	uint8_t *name;
	R_DATA *resource;
	uint8_t *rdata;
	int pref;

};
 
typedef struct
{
	uint8_t *name;
	QUESTION *ques;

} QUERY;



#endif // DUG_STRUCTURES_H
