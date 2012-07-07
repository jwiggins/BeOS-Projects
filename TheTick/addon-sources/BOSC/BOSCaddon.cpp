/*
	BOSCaddon.cpp - BeOS Central NewsTicker add-on
	
	TheTick by John Wiggins. 1999

*/


#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <SupportDefs.h>
#include "TickerAddon.h"

extern "C" 
{
	_EXPORT void parse(const char *buffer, int32 bufferlen, char ***output, char ***outlinks, int32 *outnum);
	_EXPORT TickerAddonInfo Info = { "www.beoscentral.com", "/powerbosc.txt", "BeOS Central", (bigtime_t)30*60*1000*1000 /* 30 minutes */ };
}

void parse(const char *buffer, int32 bufferlen, char ***output, char ***outlinks, int32 *outnum)
{
	//printf("Entering addon's parse() function.\n");
	//printf("%s\n", buffer);
	//printf("bufferlen = %d\n", bufferlen);
	
	const char *ptr = NULL;
	int32 num_strings = 0;
	
	ptr = strstr(buffer, "\r\n\r\n"); // move to the beginning of the end of the http header
	ptr += 4; // move past that
	
	bufferlen -= (ptr - buffer - 1); // adjust bufferlen
	buffer = ptr;
	
	//printf("%d\n", buffer[0]);
	//printf("bufferlen = %d\n", bufferlen);
	//printf("strlen(ptr) = %d\n", strlen(ptr));
		
	//printf("sizeof(output) = %d\nsizeof(*output) = %d\n",
	//		sizeof(output), sizeof(*output));
	
	//printf("parsing buffer...\n");
	
	// count the strings
	ptr = buffer; // not really needed. clarity only.
	while(ptr != NULL)
	{
		ptr = strstr(ptr, "%%\n");
		if(ptr != NULL)
		{
			ptr += 3;
			num_strings++;
		}
		else
			break;
	}
	
	//printf("num_strings = %d\n", num_strings);
	
	*outnum = num_strings; // assign to outnum
	
	(*output) = (char **)malloc(((*outnum) + 1) * sizeof(char *)); // duh! pointers are 32 bits!
													// (MALLOC_DEBUG whipped me for a couple
													// days over this). Talk about your
													// hard bugs.... I'm so used to 
													// allocating space related to strings
													// in single bytes...
	memset((void *)(*output), 0, (((*outnum) + 1) * sizeof(char *))); // memset for good measure
	
	// same for outlinks
	(*outlinks) = (char **)malloc(((*outnum) + 1) * sizeof(char *));
	memset((void *)(*outlinks), 0, (((*outnum) + 1) * sizeof(char *)));
	
	// now parse out all them bad boys!
	ptr = buffer; // back to the start of the buffer
	for(int i=0;i<(*outnum);i++)
	{
		//printf("parse loop. string %d\n", i);
		const char *start = NULL, *end = NULL;
		int32 len=0;
		
		//printf("start = strstr()\n");
		start = strstr(ptr, "%%\n");
		start += 3;
		//printf("end = strstr()\n");
		end = strstr(start, "\n");
		end += 1;
		
		len = (end - start) + 1; // NULL + an extra space at the end
		//printf("len = %d\n", len);
		
		(*output)[i] = (char *)malloc(len);
		memset((void *)(*output)[i], 0, len);
		strncpy((*output)[i], start, (len-1));
		(*output)[i][len-2] = ' ';
		
		//printf("(*output)[%d] = %s\n", i, (*output)[i]);
		
		// and then the link
		
		start = end;
		
		end = strstr(start, "\n");
		
		len = (end - start) + 1;
		
		(*outlinks)[i] = (char *)malloc(len);
		memset((void *)(*outlinks)[i], 0, len);
		strncpy((*outlinks)[i], start, (len-1));
		
		ptr = end + 1;
	
	}	
	(*output)[(*outnum)] = NULL;
	(*outlinks)[(*outnum)] = NULL;
	
	//for(int32 i=0;i<(*outnum);i++)
	//{
	//	printf("(*output)[%d] = %s\n", i, (*output)[i]);
	//}
	//printf("Exiting addon's parse() function.\n");
}