#include "resolver.h"



int something_is_in_error(DNS_HEADER* dns_header)
{

	// Covers most basic errors and prints likely cause
	// These won't be debug mode since we should always show error
	if (dns_header->rcode != 0)
	{
		printf(KRED "rcode error, not good" KRESET "\n");
		return -1;
	}
	if (dns_header->aa == 1 && dns_header->ans_count == 0)
	{
		printf(KRED "this domain does not exist, not good" KRESET "\n");
		return -1;
	}
	if (dns_header->auth_count == 0 && dns_header->ans_count == 0
									&& dns_header->add_count == 0)
	{
		printf(KRED "no records found for this domain" KRESET "\n");
		return -1;
	}

	return 0;

}

unsigned char* get_name(uint8_t* reader, uint8_t* buf, int* count)
{
    unsigned char* hostname = (unsigned char*) malloc(512);
    uint8_t loc = 0, i , j = 0;
    *count = 1;
    hostname[0]='\0';
 
    while(*reader!= 0)
    {
        if(*reader >= 192)
        {
						// we have an offset to tkae into acount
						// and must reset the location of our reader
            reader = buf + *(reader + 1) - 1;
            loc = 1; //new location
        }
        else
        {
						//otherwise keep building hostname
            hostname[j++]= *reader;
        }
       
				//increment	
				if(loc == 0) *count += 1; 
        reader++;
 
    }
    hostname[j]='\0'; 
		size_t size_hostname = j;

		//conver to standard
		// number to go
		int num = 0;	
    for(i=0; i < size_hostname; i++) 
    {
        num  = hostname[i];
        for(j=0; j < num; j++, i++) 
            hostname[i] = hostname[i+1];
        hostname[i]='.';
    }
		hostname[i-1] = '\0';
    
		if(loc == 1) *count += 1;

    return hostname;
}



// not sure if i need this bullshit
void decode_header(DNS_HEADER* header, uint16_t value)
{
    // Because of byte ordering, we have to do this nasty ranged bit mask to get it to proper byte order
    header->qr = (uint16_t)(value >> 15);
    header->opcode = (uint16_t)((value >> 11) & ~((uint16_t)~0 << 4));
    header->aa = (uint16_t)((value >> 10) & ~((uint16_t)~0 << 1));
    header->tc = (uint16_t)((value >> 9) & ~((uint16_t)~0 << 1));
    header->rd = (uint16_t)((value >> 8) & ~((uint16_t)~0 << 1));
    header->ra = (uint16_t)((value >> 7) & ~((uint16_t)~0 << 1));
    header->z = (uint16_t)((value >> 6) & ~((uint16_t)~0 << 3));
    header->rcode = (uint16_t)((value >> 0) & ~((uint16_t)~0 << 4));
}

void formatToDNS(unsigned char* dns, char* host) 
{
    int lock = 0 , i;
		char* mod_host = (char*) malloc(sizeof(char) * strlen(host));
		sprintf(mod_host, "%s.", host);
     
    for(i = 0 ; i < strlen(mod_host) ; i++) 
    {
        if(mod_host[i]=='.') 
        {
            *dns++ = i-lock;
            for(;lock<i;lock++) 
            {
                *dns++=mod_host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *dns++='\0';
}


uint8_t* query_dns_server(unsigned char *host, char* name_server,
																						int query_type)
{

	/* DNS FORMAT send
	 * ===============
	 * header (DNS_HEADER struct)
	 * QUESTION (qname is separate from QUESTION struct)
	 * 
	 * DNS FORMAT answer
	 * ================
	 * Answer
	 * Authority
	 * Additional
	 * (each of above can be represented wtih a RES_RECORD struct)
	 */

		//can't malloc this buffer just allocate a lot and call it a day
		uint8_t send_buf[2048];

		// This socket will open port 53 and send stuffs and receive
    struct sockaddr_in dest;

		int sockfd = socket(PF_INET , SOCK_DGRAM , IPPROTO_UDP);
 
    dest.sin_family = PF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(name_server); //dns servers
 
    //Set the DNS structure to standard queries
    DNS_HEADER* dns_header = (DNS_HEADER*)malloc(sizeof(DNS_HEADER));

    dns_header->id = htons(getpid());
    dns_header->qr = 0; //This is a query
    dns_header->opcode = 0; //This is a standard query
    dns_header->aa = 0; //Not Authoritative
    dns_header->tc = 0;
    dns_header->rd = 0; //dun do recursion (we coded it)
    dns_header->ra = 0; 
    dns_header->z = 0;
    dns_header->rcode = 0;
    dns_header->q_count = ntohs(1); //we have only 1 question
    dns_header->ans_count = 0;
    dns_header->auth_count = 0;
    dns_header->add_count = 0;
    
		
		uint8_t* qname = (uint8_t*) malloc(sizeof(uint8_t) * strlen(host));
    formatToDNS(qname, host);

		size_t header_size = sizeof(DNS_HEADER);
		size_t query_size = strlen((char*)qname) + 1;

    QUESTION* question =(QUESTION*) malloc(sizeof(QUESTION));
    question->qtype = htons( query_type );
    question->qclass = htons(1);

		int total_size= 0;

		memcpy(send_buf,dns_header,sizeof(DNS_HEADER));
		total_size += sizeof(DNS_HEADER);

		memcpy(send_buf+total_size, qname,strlen(qname) + 1);
		total_size += strlen(qname) + 1;

		memcpy(send_buf+total_size, question,sizeof(QUESTION));
		total_size += sizeof(QUESTION);

		socklen_t sock_size = sizeof(dest);
		debug(KYEL	"Sending Packet..." KRESET "\n");
    if( sendto(sockfd, (char*)send_buf, total_size, 0,
					(struct sockaddr*)&dest, sock_size) < 0)
    {
        perror("sendto failed");
    }

     
    //Receive the answer
		uint8_t* ans_buf = (uint8_t*) malloc(sizeof(uint8_t)*512);
    if(recvfrom (sockfd,(char*)ans_buf , 512, 0 , 
					(struct sockaddr*)&dest , &sock_size) < 0)
    {
        perror("recvfrom failed");
    }
    debug(KYEL "received packet" KRESET "\n");

		return ans_buf;
}

	
void run_program(unsigned char* hostname, char* type_str, char* dns_server)
{
	struct sockaddr_in a;

	char record_type_map[20][8];
	strcpy(record_type_map[1], "A");
	strcpy(record_type_map[2], "NS");
	strcpy(record_type_map[5], "CNAME");
	strcpy(record_type_map[6], "SOA");
	strcpy(record_type_map[15], "MX");
	
	int i, j;

	// determine type of number we should send in query type
	for (i = 0; i < 20; i++)
	{
		if (strcmp(record_type_map[i], type_str) == 0)
			break;
	}
	int query_type = i; //A type for now


	// parse hostname into a recursive domain thingy
	// ie imagine.mines.edu we do
	// 1. query edu.
	// 2. query mines.edu.
	// 3. query imagine.mines.edu.
	// make sure each above use proper nameserver (start at root A server)
	// maybe later optimize for it to use initial nameserver first



	char* curr_nameserver = ROOT_SERVER;


	// array of all possible nameservers to try next iteration
	char possible_next_namservers[20][30];

	// all the NS record names found
	// we will try to find an ip associated with one of them
	char list_NS[20][30];



	int answerFound = 0;

	DNS_HEADER* dns_header = NULL;


	char* hostname_tokens[30];
	i = 0;
	hostname_tokens[i] = strtok(hostname, ".");

	while (hostname_tokens[i] != NULL)
	{
		hostname_tokens[++i] = strtok(NULL, ".");
	}
	size_t size_hostname_token = i;

	
	struct RES_RECORD answers[20],auth[20],addit[20];
	uint8_t* ans_buf;

while (!answerFound)
{
	for (int k = size_hostname_token - 1; k >= 0; k--)
	{
	

		memset(&answers[0], 0, sizeof(answers));
		memset(&addit[0], 0, sizeof(addit));
		memset(&auth[0], 0, sizeof(auth));


		unsigned char curr_hostname[60];

		int pos = 0;
		for (i = k; i < size_hostname_token; i++)
		{
			pos += sprintf(&curr_hostname[pos], "%s.", hostname_tokens[i]);
		}
	
		int curr_query_type = 2;
		if (k == 0)
			curr_query_type = query_type;

		debug(KBLUE " QUERYING HOSTNAME %s" KRESET "\n", curr_hostname);
		debug(KBLUE " QUERYING NAMESERVER %s" KRESET "\n", curr_nameserver);
		debug(KBLUE " QUERY TYPE %s" KRESET "\n", record_type_map[curr_query_type]);

	//does the shit
	//ans_buf = query_dns_server(curr_hostname, curr_nameserver, query_type);


	// make a qname record to calculate total size for the reader
	// to go to right spot
	char dummy_qname[50];
	formatToDNS(dummy_qname, curr_hostname);
		
	size_t header_size = sizeof(DNS_HEADER);
	size_t query_size = strlen((char*)dummy_qname) + 1;
	size_t question_size = sizeof(QUESTION);
	size_t total_size = header_size + query_size + question_size;


	// always query as an NS until we get to final query then
	// query what user wants
	ans_buf = query_dns_server(curr_hostname, curr_nameserver, curr_query_type);

	// repoint dns header to answer buffer since we should
	// have some values now
	dns_header = (DNS_HEADER*) ans_buf; 

	// change bits (compensates errors in wireshark)
	decode_header(dns_header, ntohs(((ans_buf[3] << 8) | ans_buf[2])));

	if (something_is_in_error(dns_header) == -1)
	{
		exit(-1);
	}

	//ptr that goes through ans_buf
	uint8_t* read_ptr;
	read_ptr = &ans_buf[total_size]; //start after dns header

	int offset = 0; //keeps track of compression

	//READ EVERYTHING
	//===============

	//read answers
	for (i = 0; i < ntohs(dns_header->ans_count); i++)
	{
		answers[i].name = get_name(read_ptr, ans_buf, &offset);
		read_ptr += offset;

		answers[i].resource = (R_DATA*)(read_ptr);
		read_ptr += sizeof(R_DATA);

		int record_type = ntohs(answers[i].resource->type);
		size_t len = ntohs(answers[i].resource->data_len);
		switch (record_type) 
		{
			case 1: //A type record
				answers[i].rdata = (uint8_t*) malloc(len);
				for (j = 0; j < len; j++)
					answers[i].rdata[j] = read_ptr[j];
				answers[i].rdata[j] = '\0';
				read_ptr += len;
				break;

			case 15: //T_MX
				answers[i].pref = read_ptr[1] + 256 * read_ptr[0];
				read_ptr += 2;
				answers[i].rdata = get_name(read_ptr, ans_buf, &offset);
				read_ptr += offset;
				break;
			default:
				answers[i].rdata = get_name(read_ptr, ans_buf, &offset);
				read_ptr += offset;
		}
	}


	//read authorities
	for (i = 0; i < ntohs(dns_header->auth_count); i++)
	{
		auth[i].name = get_name(read_ptr, ans_buf, &offset);
		read_ptr += offset;

		// populate r_DATA in the auth
		auth[i].resource=(R_DATA*)(read_ptr);
		read_ptr += sizeof(R_DATA);

		auth[i].rdata = get_name(read_ptr, ans_buf, &offset);
		read_ptr += offset;
	}

	//read additional
	for (i = 0; i < ntohs(dns_header->add_count); i++)
	{
		addit[i].name = get_name(read_ptr, ans_buf, &offset);
		read_ptr += offset;

		addit[i].resource = (R_DATA*)(read_ptr);
		read_ptr += sizeof(R_DATA);

		int record_type = ntohs(addit[i].resource->type);
		size_t len = ntohs(addit[i].resource->data_len);
		if (record_type == 1)
		{
				addit[i].rdata = (uint8_t*) malloc(sizeof(uint8_t) * len);
				for (j = 0; j < len; j++)
					addit[i].rdata[j] = read_ptr[j];
				addit[i].rdata[j] = '\0';
				read_ptr += len;
		}
		else
		{
			addit[i].rdata = get_name(read_ptr,ans_buf,&offset);
			read_ptr = read_ptr + offset;
		}
	}


	// DECIDE NEXT NAMESERVER (OR SEE IF WE ARE DONEZO)
	//==================================================

	for(i = 0; i < ntohs(dns_header->auth_count); i++)
	{
		strcpy(list_NS[i], auth[i].rdata);
	}

	for(i = 0; i < ntohs(dns_header->add_count); i++)
	{
		// read record
		int record_type = ntohs(addit[i].resource->type);

		long *p;
		// we want A records so we get some ips
		if (record_type == 1)
		{
			p=(long*)addit[i].rdata;
			a.sin_addr.s_addr=(*p);
			char* ipv4_addr = inet_ntoa(a.sin_addr);

			for (j = 0; j < ntohs(dns_header->auth_count); j++)
			{
				if (strcmp(list_NS[j], addit[i].name) == 0)
				{
					// make a list of nameservers that should work
					// for are next query
					strcpy(possible_next_namservers[j], ipv4_addr);
				}
			}
		}
	}

	answerFound = (ntohs(dns_header->ans_count) > 0) ? 1 : 0;

	if (!answerFound)
	{
		curr_nameserver = possible_next_namservers[0];
		debug(KGREEN "NEXT NAMESERVER TO QUERY %s" KRESET "\n", curr_nameserver);
	}
	else
		debug(KGREEN "FINISHED" KRESET "\n");
	} //end for loop 

}

	printf("\n");
	digprint("Query Processed!\n");
	digprint("================\n");
	digprint("QUERY: %d, ",ntohs(dns_header->q_count));
	digprint("ANSWER: %d, ",ntohs(dns_header->ans_count));
	digprint("AUTHORITY: %d, ",ntohs(dns_header->auth_count));
	digprint("ADDITIONAL: %d\n",ntohs(dns_header->add_count));



	//initialize a final authoritize answer (like assignment handout)
	char query_answer[40];
	int isAuthority = 0;

	// PRINT THE OUTPUT HERE
	//=======================

	printf("\n\n");

	//print answers
	digprint(";;  ANSWER SECTION:\n");
	for (i = 0; i < ntohs(dns_header->ans_count); i++)
	{
		// read record
		int record_type = ntohs(answers[i].resource->type);

		long *p;
		switch (record_type)
		{
			case 1:
				p=(long*)answers[i].rdata;
				a.sin_addr.s_addr=(*p);
				char* ipv4_addr = inet_ntoa(a.sin_addr);
				digprint("%s\tA\t%s\n", answers[i].name, ipv4_addr);
				strcpy(query_answer, ipv4_addr);
				isAuthority = 1;
				break;
			case 2:
				digprint("%s\tNS\t%s\n", answers[i].name, answers[i].rdata);
				char* new_str2 = (char*) malloc(strlen(answers[i].rdata) + 5);
				sprintf(new_str2, "%s",answers[i].rdata);
				strcpy(query_answer, new_str2);
				break;
			case 5:
				digprint("%s\tCNAME\t%s\n", answers[i].name, answers[i].rdata);
				char* new_str1 = (char*) malloc(strlen(answers[i].rdata) + 5);
				sprintf(new_str1, "%s",answers[i].rdata);
				strcpy(query_answer, new_str1);
				break;
			case 15:
				digprint("%s\tMX\t%d %s\n", answers[i].name, answers[i].pref, answers[i].rdata);
				char* new_str = (char*) malloc(strlen(answers[i].rdata) + 5);
				sprintf(new_str, "%d %s", answers[i].pref, answers[i].rdata);
				strcpy(query_answer, new_str);
				break;
			default:
				digprint("un supported cord type %d\n", record_type);
		}

	}

	//print authorities
	digprint(";;  AUTHORITY SECTION:\n");
	for (i = 0; i < ntohs(dns_header->auth_count); i++)
	{
		digprint("%s\n", auth[i].rdata);
		int record_type = ntohs(auth[i].resource->type);
		digprint("%s\t%s\t%s\n", auth[i].name, record_type_map[record_type], auth[i].rdata);

	}

	digprint(";;  ADDITIONAL SECTION:\n");
	for (i = 0; i < ntohs(dns_header->add_count); i++)
	{
		// read record
		int record_type = ntohs(addit[i].resource->type);

		long *p;
		if (record_type == 1)
		{
			p=(long*)addit[i].rdata;
			a.sin_addr.s_addr=(*p);
			char* ipv4_addr = inet_ntoa(a.sin_addr);

			digprint("%s\tA\t%s\n", addit[i].name, ipv4_addr);
		}
		else if (record_type == 5) //CNAME
		{
			digprint("%s\tCNAME\t%s\n", addit[i].name, addit[i].rdata);
		}
		else 
		{
			// record type may just be unrecognized (dont have AAAA)
			// or we just have some data loss (not big deal for this simple server)
			digprint("record type unrecognized or corrupted\n");
		}
	}



	if (isAuthority)
	{
		printf(KGREEN "Authoritative answer: %s" KRESET "\n", query_answer);

		if (cacheResults)
		{
			FILE *fp;
			fp=fopen("cache.txt", "a");
			if(fp == NULL)
			{
				printf(KRED "cache is broken somehow" KRESET "\n");
				exit(-1);
			}

			fprintf(fp, "Authoritative answer: %s\n", query_answer);
			fclose(fp);
		}
	}
	else
	{
		printf(KGREEN "Non-authoritative answer: %s" KRESET "\n", query_answer);

		if (cacheResults) 
		{
			FILE *fp;
			fp=fopen("cache.txt", "a");
			if(fp == NULL)
			{
				printf(KRED "cache is broken somehow" KRESET "\n");
				exit(-1);
			}

			fprintf(fp, "Non-authoritative answer: %s\n", query_answer);
			fclose(fp);
		}
	}

	//free answer
	free(ans_buf);

}
		
