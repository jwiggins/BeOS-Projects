/*

	ArrtItem.h
	John Wiggins 1998

*/

#ifndef ATTR_ITEM_H
#define ATTR_ITEM_H

#include <Region.h>
#include <Font.h>
#include <View.h>
#include <Rect.h>
#include <string.h>
#include <image.h>

#include "CLVListItem.h"
#include "Colors.h"

class AttrItem : public CLVListItem
{
	public:
					AttrItem(const char* text0, const char* text1, bool has_addon, image_id addon_id);
					~AttrItem();
		void 		DrawItemColumn(BView* owner, BRect item_column_rect, int32 column_index, bool complete);
		void 		Update(BView *owner, const BFont *font);
		bool 		HasAddon() const {return _has_addon;}
		const char	*AttrName() const {return fText[0];}
		const char	*TypeName() const {return fText[1];}
		image_id	Id() const {return id;}

	private:
		char*		fText[2];
		bool		_has_addon;
		image_id	id;
		float		fTextOffset;
};

#endif