#include "globals.h"

// initializing on the globals, by default you will get a more
// "dig-like output". Other features need to be flagged manually


int isVerbose = 0; 
int isDig = 1; 
int isMattDaemon = 0; 
int cacheResults = 0; 

void debug(char* format, ...) 
{
	if (isVerbose) 
	{
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
	}
}

void cacheprint(char* format, ...) 
{
	va_list args;
	if (isDig) 
	{
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
	}
}

void digprint(char* format, ...) 
{
	va_list args;
	if (isDig) 
	{
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
	}

	if (cacheResults)
	{
		char* result = (char*)malloc(strlen(format) + sizeof(args) + 200);
		va_start(args, format);
		vsprintf(result, format, args);
		va_end(args);

		FILE *fp;
		fp=fopen("cache.txt", "a");
		if(fp == NULL)
		{
			printf(KRED "cache is broken somehow" KRESET "\n");
			exit(-1);
		}

		//extroadinarly stupid code
		if (strcmp(result, "Query Processed!\n") == 0)
			fprintf(fp, "Query Cached!\n");
		else
			fprintf(fp, "%s", result);
		fclose(fp);
	}

}
