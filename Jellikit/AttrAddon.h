#ifndef ATTR_ADD_ON_H
#define ATTR_ADD_ON_H

#include <Archivable.h>
#include <View.h>
#include <Rect.h>
#include <SupportDefs.h>

// Instantiate Message contains:
//	- "typecode" - type_code of data to be edited.
//	- "data" - a pointer to data you should edit. You need to make your own copy.
//	- "datasize" - int32 telling you how big the data is (in bytes)

/* example export. add yer own type_codes
extern "C" 
{
	_EXPORT AttrAddon * Instantiate(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret);
	_EXPORT type_code TypeCode[] = { (type_code)NULL };
}
*/

// empty base class : derive your own and override all functions
class AttrAddon : public BView {
	public:
					AttrAddon(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *, status_t *ret)
					:BView(frame,name,resizeMask,flags) {*ret = B_ERROR;};
virtual				~AttrAddon() {;}; // cleanup
virtual	status_t	GetData(BMessage *){return B_ERROR;};
virtual	status_t	ChangeData(BMessage *){return B_ERROR;};
virtual	bool		IsDirty() const {return false;};

};

#endif