// define basic DNS Header and Question structures
// as copied from the slides in class

#include <stdint.h>

#ifndef DUG_STRUCTURES_H
#define DUG_STRUCTURES_H

typedef struct
{
	uint16_t id;       
	uint8_t rd :1;     
	uint8_t tc :1;     
	uint8_t aa :1;     
	uint8_t opcode :4; 
	uint8_t qr :1;     
	uint8_t rcode :4;  
	uint8_t z :3;      
	uint8_t ra :1;     
	uint16_t q_count;  
	uint16_t ans_count; 
	uint16_t auth_count; 
	uint16_t add_count; 

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
