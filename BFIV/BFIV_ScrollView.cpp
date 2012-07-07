#include "BFIV_ScrollView.h"

BFIV_ScrollView::BFIV_ScrollView(BRect frame, BFIV_View *image)
: BView(frame, "scroller", B_FOLLOW_ALL, /*B_WILL_DRAW|*/B_FRAME_EVENTS)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	AddChild(fImage = image); // add and cache pointer
	
	// scoot the image container down and over to make it prettier
	fImage->MoveBy(5, 5);
}

//void	BFIV_ScrollView::Draw(BRect)
//{
//	BRect rect = fImage->Frame();
//	rect.InsetBy(-1.0, -1.0);
//	StrokeRect(rect, B_SOLID_HIGH);
//}

void BFIV_ScrollView::AllAttached()
{
	ResizeTo((Window()->Bounds().Width() - B_V_SCROLL_BAR_WIDTH),
		(Window()->Bounds().Height() - Frame().top - B_H_SCROLL_BAR_HEIGHT));
}

void BFIV_ScrollView::FrameResized(float, float)
{
	DoScrollBars();
}

void BFIV_ScrollView::DoScrollBars()
{
	// use BView::ScrollBar(B_VERTICAL||B_HORIZONTAL) to get scroll bars
	BRect bounds = Bounds(), ibounds = fImage->Bounds();
	float curWidth = bounds.Width(), curHeight = bounds.Height();
	float maxWidth = ibounds.Width(), maxHeight = ibounds.Height();
	float propHor, propVert, rangeHor, rangeVert;
	BScrollBar *scroll;
	
	//bounds.PrintToStream();
	//ibounds.PrintToStream();
	
	// 5 pixel gutter on each side
	maxWidth += 10;
	maxHeight += 10;
	
	// proportional width and height
	propHor = curWidth/maxWidth;
	propVert = curHeight/maxHeight;
	
	// width and height ranges
	rangeHor = maxWidth - curWidth;
	rangeVert = maxHeight - curHeight;
	
	// sanity check
	if(rangeHor < 0.0) rangeHor = 0.0;
	if(rangeVert < 0.0) rangeVert = 0.0;
	
	// update scrollbars
	if ((scroll = ScrollBar(B_HORIZONTAL)) != NULL)
	{
		scroll->SetProportion(propHor);
		scroll->SetRange(0,rangeHor);
		scroll->SetSteps(curWidth / 20.0, curWidth / 5.0);
	} 
	if ((scroll = ScrollBar(B_VERTICAL)) != NULL)
	{
		scroll->SetProportion(propVert);
		scroll->SetRange(0,rangeVert);
		scroll->SetSteps(curHeight / 20.0, curHeight / 5.0);
	}
}