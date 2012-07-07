#ifndef STRING_ADDON_H
#define STRING_ADDON_H
/*
	include every friggin header you can imagine, plus 2 or 3 extra 
	to appease the GUH-NOO C Compiler.
*/
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
#include <stdlib.h>

#include "AttrAddon.h"

extern "C" 
{
#if B_BEOS_VERSION < B_BEOS_VERSION_4 /* revert to declsuck for pre R4 */
	__declspec(dllexport) AttrAddon * Instantiate(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret);
	__declspec(dllexport) type_code TypeCode[] = { B_BOOL_TYPE, (type_code)NULL };
#else
	_EXPORT AttrAddon * Instantiate(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret);
	_EXPORT type_code TypeCode[] = { B_BOOL_TYPE, (type_code)NULL };
#endif
}

class BoolAddon : public AttrAddon {
	public:
					BoolAddon(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret);
					
virtual				~BoolAddon(); // cleanup
virtual	status_t	GetData(BMessage *msg);
virtual	status_t	ChangeData(BMessage *msg);
virtual	bool		IsDirty() const;
	
	private:
type_code			current_type;
bool				initial_value;

};

#endif