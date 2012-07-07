//CLVListItem source file


//******************************************************************************************************
//**** PROJECT HEADER FILES
//******************************************************************************************************
#define CLVListItem_CPP
#include "CLVListItem.h"
#include "ColumnListView.h"
#include "CLVColumn.h"


//******************************************************************************************************
//**** CLVItem CLASS DEFINITION
//******************************************************************************************************
CLVListItem::CLVListItem(uint32 level, bool _UNUSED(superitem), bool expanded, float minheight)
: BListItem(level, expanded)
{
	fMinHeight = minheight;
}


CLVListItem::~CLVListItem()
{ }


void CLVListItem::DrawItem(BView* owner, BRect itemRect, bool complete)
{
	BList* DisplayList = &((ColumnListView*)owner)->fColumnDisplayList;
	int32 NumberOfColumns = DisplayList->CountItems();
	//float PushMax = itemRect.right;
	CLVColumn* ThisColumn;
	BRect ThisColumnRect = itemRect;
	
	BRegion ClippingRegion;
	if(!complete)
		owner->GetClippingRegion(&ClippingRegion);
	else
		ClippingRegion.Set(itemRect);
	float LastColumnEnd = -1.0;

	//Draw the columns
	for(int32 Counter = 0; Counter < NumberOfColumns; Counter++)
	{
		ThisColumn = (CLVColumn*)DisplayList->ItemAt(Counter);
		if(!ThisColumn->IsShown())
			continue;
		ThisColumnRect.left = ThisColumn->fColumnBegin;
		ThisColumnRect.right = LastColumnEnd = ThisColumn->fColumnEnd;
		if(ThisColumnRect.right >= ThisColumnRect.left && ClippingRegion.Intersects(ThisColumnRect))
			DrawItemColumn(owner, ThisColumnRect, ((ColumnListView*)owner)->fColumnList.IndexOf(ThisColumn),complete);
	}
	//Fill the area after all the columns (so the select highlight goes all the way across)
	ThisColumnRect.left = LastColumnEnd + 1.0;
	ThisColumnRect.right = owner->Bounds().right;
	if(ThisColumnRect.left <= ThisColumnRect.right && ClippingRegion.Intersects(ThisColumnRect))
		DrawItemColumn(owner, ThisColumnRect,-1,complete);
}

void CLVListItem::Update(BView* owner, const BFont* font)
{
	BListItem::Update(owner,font);
	float ItemHeight = Height();
	if(ItemHeight < fMinHeight)
		ItemHeight = fMinHeight;
	SetWidth(((ColumnListView*)owner)->fPageWidth);
	SetHeight(ItemHeight);
}
