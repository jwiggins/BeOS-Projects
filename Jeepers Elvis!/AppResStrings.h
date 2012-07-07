/*
	AppResStrings.h
	Convenience class for localization
	2000 John Wiggins
*/

#ifndef APP_RES_STRINGS_H
#define APP_RES_STRINGS_H

// interface 
#include <InterfaceDefs.h>
// storage kit
#include <ResourceStrings.h>
// support kit
#include <List.h>
// posix
#include <stdio.h>
#include <malloc.h>
#include <string.h>
// mine
#include "AppResStringDefs.h"

class BList;

class AppResStrings {
public:
				AppResStrings();
				~AppResStrings();
const char		*String(int32 id) const
				{
					return (const char *)StringList->ItemAt(id);
				}

private:
BList			*StringList;

};

#endif
