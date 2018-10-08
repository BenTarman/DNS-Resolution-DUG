// Code for daemon
#include "matt_daemon.h"
#include "resolver.h"
#include <time.h>

void matt_daemon(const char* nameserver)
{
		int local_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		struct sockaddr_in server;
	
		// assign random port number
		srand (time(NULL));
		int port = (rand() % (10000 - 1 + 1)) + 1; 
		char port_str[20];
		sprintf(port_str, "Using port %d", port);
		puts(port_str);

		//have a socket listen on localhost
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = htonl(INADDR_ANY);
		server.sin_port = htons(port);

		if ((bind(local_sock, (struct sockaddr *)&server,
						sizeof(server)) < 0))
		{
			debug(KRED "local server socket couldn't bind" KRESET "\n");
			exit(-1);
		}

			// this buffer will receive information from outside
			// programs such as dig
			uint8_t buffer[512];
			socklen_t addrLen = sizeof(server);

			// just do an infinite loop so we listen
			//while (1) {
				if ((recvfrom(local_sock, buffer, 512, 0, 
						(struct sockaddr*)&server, &addrLen)) < 0)
				{
					debug(KRED "couldn't receive packets" KRESET "\n");
					exit(-1);
				}

				process_matt_daemon((char*)nameserver, buffer);
			//}


}


void process_matt_daemon(char* dns_server, uint8_t* ans_buf)
{
	char record_type_map[20][8];
	strcpy(record_type_map[1], "A");
	strcpy(record_type_map[2], "NS");
	strcpy(record_type_map[5], "CNAME");
	strcpy(record_type_map[6], "SOA");
	strcpy(record_type_map[15], "MX");

	uint8_t* read_ptr;
	// when dig sends we have domain name after headers
	read_ptr = &ans_buf[sizeof(DNS_HEADER)];
	int stop = 0;

	char* hostname = get_name(read_ptr, ans_buf, &stop);
	read_ptr += stop;
	
	int query_type_pos = sizeof(DNS_HEADER) + stop + 1;
	char* type_str = record_type_map[ans_buf[query_type_pos]];

	// Before going on check if we want to use a cache
	if (cacheResults)
	{
		int isCached = cache(hostname, dns_server, type_str);
		if (isCached)
			exit(0);

		FILE *fp;
		fp=fopen("cache.txt", "a");
		if(fp == NULL)
		{
			printf(KRED "cache is broken somehow" KRESET "\n");
			exit(-1);
		}

		fprintf(fp, "* %s %s %s\n", hostname, dns_server, type_str);
		fclose(fp);
	}
		


	printf("%s\n", type_str);
	printf("%s\n", hostname);

	run_program(hostname, type_str, dns_server);
}

