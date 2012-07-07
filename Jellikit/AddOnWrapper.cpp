/*

	AddOnWrapper.cpp - a nicer interface to add-ons
	John Wiggins 1998

*/

#include "AddOnWrapper.h"

AddOnWrapper::AddOnWrapper(image_id id)
			:addon_id(id), initialized(false)
{
	if(get_image_symbol(id, "TypeCode", B_SYMBOL_TYPE_DATA, (void **)&_TypeCode))
	{
		//printf( "get_image_symbol( TypeCode ) failed\n" );
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
	// the add-ons are destroyed when the addon manager empties its list of 
	// AddOnWrappers. It will check to make sure all windows are gone before
	// doing this, thus insuring that some view out there doesn't get its code yanked
	// out from under it.
	_TypeCode = NULL;
	unload_add_on(addon_id); // unload!
}

bool AddOnWrapper::SupportsType(type_code *type) const
{
	int32 j=0;
	while((_TypeCode[j] != *type) && (_TypeCode[j]/*!= NULL*/))
			j++;
	if(_TypeCode[j] == *type)
		return true;
	else
		return false;
}