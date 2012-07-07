/*
	AddOnWrapper.cpp
	
	TheTick by John Wiggins. 1999

*/

#include "AddOnWrapper.h"

AddOnWrapper::AddOnWrapper(image_id id)
			:addon_id(id), initialized(false)
{
	if(get_image_symbol(id, "Info", B_SYMBOL_TYPE_DATA, (void **)&addon_info) < B_NO_ERROR)
	{
		//printf( "get_image_symbol( Info ) failed\n" );
		unload_add_on(id);
	}
	else if(get_image_symbol(id, "parse", B_SYMBOL_TYPE_TEXT, (void **)&parse_func) < B_NO_ERROR)
	{
		//printf( "get_image_symbol( parse ) failed\n" );
		unload_add_on(id);
	}
	else
	{
		// woohoo. 
		initialized = true;
	}
}

AddOnWrapper::~AddOnWrapper()
{
	addon_info = NULL;
	unload_add_on(addon_id); // unload!
	//printf("AddOnWrapper dtor\n");
}

void AddOnWrapper::parse(const char *buffer, int32 bufferlen, char ***output, char ***outlinks, int32 *outnum)
{
	(*parse_func)(buffer, bufferlen, output, outlinks, outnum);
}