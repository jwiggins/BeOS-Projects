/*

	AddOnWrapper.h
	John Wiggins 1998

*/

#ifndef ADDON_WRAPPER_H
#define ADDON_WRAPPER_H

#include <Rect.h>
#include <Message.h>
#include <SupportDefs.h>
#include <image.h>
#include <stdio.h>

// typedefs of addon exported functions
//typedef AttrAddon * (*attr_addon_instantiation_func) (BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg);

class	AddOnWrapper	{
	
public:
					AddOnWrapper(image_id id);
					~AddOnWrapper();
bool				InitCheck() const { return initialized; } // tells us if all the symbols loaded ok
bool				SupportsType(type_code *type) const;
image_id			Id() const { return addon_id; }

private:
image_id			addon_id;
bool				initialized;
type_code 			*_TypeCode;

};

#endif