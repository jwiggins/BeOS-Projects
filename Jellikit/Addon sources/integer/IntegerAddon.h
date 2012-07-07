#ifndef INTEGER_ADDON_H
#define INTEGER_ADDON_H

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
#ifndef B_BEOS_VERSION_4 /* BeOS R3 declsuck*/
	__declspec(dllexport) extern AttrAddon * Instantiate(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret);
	__declspec(dllexport) extern type_code TypeCode[] = { /*B_INT64_TYPE,*/ B_INT32_TYPE, B_INT16_TYPE, B_INT8_TYPE, (type_code)NULL };
#else /* BeOS R4 or greater*/
	_EXPORT extern AttrAddon * Instantiate(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret);
	_EXPORT extern type_code TypeCode[] = { /*B_INT64_TYPE,*/ B_INT32_TYPE, B_INT16_TYPE, B_INT8_TYPE, (type_code)NULL };
#endif
}


class IntegerAddon : public AttrAddon {
	public:
					IntegerAddon(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret);
					
virtual				~IntegerAddon(); // cleanup
virtual	status_t	GetData(BMessage *msg);
virtual	status_t	ChangeData(BMessage *msg);
virtual	bool			IsDirty() const;
	
	private:
type_code			current_type;
int32				current_data;

};
#endif