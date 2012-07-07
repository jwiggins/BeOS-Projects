/*

	AttrItem.cpp
	John Wiggins 1998

*/

#include "AttrItem.h"

AttrItem::AttrItem(const char* text0, const char* text1, bool has_addon, image_id addon_id)
: CLVListItem(0,false,false,20.0), _has_addon(has_addon), id(addon_id)
{
	fText[0] = new char[strlen(text0)+1];
	strcpy(fText[0],text0);
	fText[1] = new char[strlen(text1)+1];
	strcpy(fText[1],text1);
}


AttrItem::~AttrItem()
{
	for(int Counter = 0; Counter <= 1; Counter++)
		delete[] fText[Counter];
}


void AttrItem::DrawItemColumn(BView* owner, BRect item_column_rect, int32 column_index, bool complete)
{
	rgb_color color;
	bool selected = IsSelected();
	if(selected)
		color = BeListSelectGrey;
	else
		color = White;
	owner->SetLowColor(color);
	owner->SetDrawingMode(B_OP_COPY);
	if(selected || complete)
	{
		owner->SetHighColor(color);
		owner->FillRect(item_column_rect);
	}
	BRegion Region;
	Region.Include(item_column_rect);
	owner->ConstrainClippingRegion(&Region);
	if((column_index >= 1))
	{
		owner->SetHighColor(Black);
		owner->DrawString(fText[column_index-1],BPoint(item_column_rect.left+5.0,item_column_rect.top+fTextOffset));
	}
	if(column_index == 0)
	{
		if(_has_addon)
		{
			owner->SetHighColor(Green);
			owner->FillEllipse(BRect(item_column_rect.left+8.0,item_column_rect.top+8.0,item_column_rect.left+13.0,item_column_rect.top+13.0));
			owner->SetHighColor(Black);
		}
	}
	owner->SetDrawingMode(B_OP_COPY);
	owner->ConstrainClippingRegion(NULL);
}


void AttrItem::Update(BView *owner, const BFont *font)
{
	CLVListItem::Update(owner,font);
	font_height FontAttributes;
	be_plain_font->GetHeight(&FontAttributes);
	float FontHeight = ceil(FontAttributes.ascent) + ceil(FontAttributes.descent);
	fTextOffset = ceil(FontAttributes.ascent) + (Height()-FontHeight)/2.0;
}
