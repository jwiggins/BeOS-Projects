#ifndef STRING_ADDON_H
#define STRING_ADDON_H

/*
	include every friggin header you can imagine, plus 2 or 3 extra 
	to appease the GUH-NOO C Compiler.
*/

#include <TextControl.h>
#include <TextView.h>
#include <ScrollView.h>
#include <View.h>
#include <StringView.h>
#include <Rect.h>
#include <Font.h>
#include <TypeConstants.h>
#include <InterfaceDefs.h>
#include <Message.h>
#include <Mime.h>
#include <SupportDefs.h>
#include <string.h>
#include <stdio.h>

#include "AttrAddon.h"


extern "C" 
{
#if B_BEOS_VERSION < B_BEOS_VERSION_4 /* BeOS R3 declsuck*/
	__declspec(dllexport) extern AttrAddon * Instantiate(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret);
	__declspec(dllexport) extern type_code TypeCode[] = { B_MIME_STRING_TYPE, B_STRING_TYPE, (type_code)NULL };
#else /* BeOS R4 or greater*/
	_EXPORT extern AttrAddon * Instantiate(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret);
	_EXPORT extern type_code TypeCode[] = { B_MIME_STRING_TYPE, B_STRING_TYPE, (type_code)NULL };
#endif
}


class StringAddon : public AttrAddon {
	public:
					StringAddon(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret);
					
virtual				~StringAddon(); // cleanup
virtual	status_t	GetData(BMessage *msg);
virtual	status_t	ChangeData(BMessage *msg);
virtual	bool		IsDirty() const;
	
	private:
type_code			current_type;
char *				initial_string;

};
#endif