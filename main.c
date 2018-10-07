#include "includes.h"
#include "resolver.h"
#include "matt_daemon.h"
#include "cache.h"

int main(int argc, char * const argv[])
{
	
	char type_str[5] = "A"; //CNAME is max which needs 5 space

	int opt = 0;
	// check if verbose mode enabled
  while ((opt = getopt(argc,argv, "d:t:s:f:c:h")) != -1) {
    
    switch (opt) {
    case 'd':
			optind--; // need to subtract if no arg (probably doing this wrongly lol)
			isVerbose = 1;
      break;
		case 't':
			sscanf(optarg, "%s", type_str);
			//if its long string they probalby are assigning hostname
			//and not an actual query type so just exit
			if (strlen(type_str) > 5)
			{
				debug(KRED "null argument my dude\n" KRESET);
				exit(-1);
			}
			break;
		case 's':
			optind--;
			isDig = 0;
			break;
		case 'f':
			optind--;
			isMattDaemon = 1;
			break;
		case 'c':
			optind--;
			cacheResults = 1;
			break;
		case 'h':
				printf("Flags for dug \n\n");
					printf("USAGE: ./dug <FLAGS> <hostname> <nameserver>\n\n");

			printf("FLAG\t\t FUNCTION\n");
			printf("=======================================\n");
			printf("-t \t set the type of record you want to query\n");
			printf("-d \t debug mode which prints verbose output of program flow\n");
			printf("-s \t 'shrinks' the output to just show answer (like handout)\n");
			printf("-h\t display help menu\n\n");

		exit(0);
			break;
    default:
			break;
    }
  }

	if (isMattDaemon)
	{
		const char* nameserver = argv[optind++];

		matt_daemon(nameserver);

		}

	else 
	{

	// so we have to make a function, that's unforunate
	const unsigned char* hostname = argv[optind++];

	printf("querying %s....\n\n", hostname);
	char* dns_server = argv[optind];


	if (cacheResults)
	{
	int isCached = cache((unsigned char*)hostname, dns_server);

	if (isCached)
		return 0;

		FILE *fp;
		fp=fopen("cache.txt", "a");
		if(fp == NULL)
		{
			printf(KRED "cache is broken somehow" KRESET "\n");
			exit(-1);
		}

		fprintf(fp, "* %s %s\n", hostname, dns_server);
		fclose(fp);
	}
		run_program((unsigned char*)hostname, type_str, dns_server);

	}

	return 0;
}



