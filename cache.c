#include "cache.h"
#include "includes.h"

int cache(unsigned char* hostname, char* dns_server, char* query_type)
{
		char* cache_result = (char*)malloc(strlen(hostname) + strlen(dns_server) + 40);
		sprintf(cache_result, "* %s %s %s\n", hostname, dns_server, query_type);

		char input[512];
		FILE *fp;
		fp=fopen("cache.txt", "r");

		if(fp == NULL)
		{
			return 0;
		}

		char cache_str[512][512];
		int cnt = 0;
		strcpy(cache_str[cnt++], "");

		while (fgets(input, sizeof(input), fp))
		{
			if (strcmp(input, cache_result) == 0)
			{
				char p = fgetc(fp);
				while (p != '*' && p != EOF)
				{
					char str[2] = "\0";
					str[0] = p;
					strcpy(cache_str[cnt++], str);
					p = fgetc(fp);
				}


			}
		}
		fclose(fp);
		free(cache_result);

	if (cnt == 1)
		return 0;

	printf(KMAG);
	for (int i = 0; i < cnt; i++)
	{
		cacheprint("%s", cache_str[i]);
	}
	printf(KRESET);

	return 1;
}
