/*
	AppResStrings.cpp
	2000 John Wiggins
*/

#include "AppResStrings.h"

AppResStrings::AppResStrings()
{
	BResourceStrings res_strings;
	StringList = new BList(STRING_END);
	const char *string_ptr = NULL;
	char *a_string = NULL;
	
	const char *default_string_table[STRING_END+1] =
	{
		"OK",
		"Cancel",
		"File",
		"About",
		"Quit",
		"Sources",
		"Mixers",
		"Filters",
		"Outputs",
		"Plugins",
		"Source 1 UI",
		"Source 2 UI",
		"Mixer UI",
		"Filter UI",
		"Output UI",
		"Source 1",
		"Source 2",
		"Default Output",
		"UI",
		"Mixer",
		"Output",
		"Email Author",
		"Options",
		"Disable Default Output",
		NULL
	};
	
	for(int32 i=0;i<STRING_END;i++)
	{
		//printf("String %d: %s\n", i, res_strings.FindString(i));
		string_ptr = res_strings.FindString(i);
		if(string_ptr != NULL)
		{
			a_string = (char *)malloc(strlen(string_ptr) + 1);
			strcpy(a_string, string_ptr);
		}
		else
		{
			a_string = (char *)malloc(strlen(default_string_table[i]) + 1);
			strcpy(a_string, default_string_table[i]);
		}
		// a copy exists regardless...
		StringList->AddItem((void *)a_string, i);
		
		a_string = NULL;
	}
}

AppResStrings::~AppResStrings()
{
	void *anItem;
	for(int32 i=0; (anItem = StringList->ItemAt(i)) != NULL; i++)
		free(anItem);
	delete StringList;
}