/*
	drawView.cpp
	John Wiggins 1998
	
*/

#include "drawView.h"

drawView::drawView(BRect frame)
		: BView(frame, "monet", B_FOLLOW_ALL, 0), bounds(frame)
{
	// normally there would be a B_WILL_DRAW flag where that 0 is
	// you leave it out for this view because it doesnt need to redraw its contents
}

void drawView::AttachedToWindow()
{
	draw(); // erase everything
	SetPenSize(2.0);
}

void drawView::draw()
{
	SetHighColor(216,216,216);
	FillRect(bounds);
	SetHighColor(20,20,20);
	Sync(); // always sync, otherwise it might be a while before you see the changes
}

void drawView::up()
{
	StrokeLine(PenLocation(), BPoint(PenLocation().x, PenLocation().y -1));
	MovePenTo(BPoint(PenLocation().x, PenLocation().y - .5));
	Sync();
}

void drawView::down()
{
	StrokeLine(PenLocation(), BPoint(PenLocation().x, PenLocation().y +1));
	MovePenTo(BPoint(PenLocation().x, PenLocation().y + .5));
	Sync();
}

void drawView::right()
{
	StrokeLine(PenLocation(), BPoint(PenLocation().x +1, PenLocation().y));
	MovePenTo(BPoint(PenLocation().x +.5, PenLocation().y));
	Sync();
}

void drawView::left()
{
	StrokeLine(PenLocation(), BPoint(PenLocation().x -1, PenLocation().y));
	MovePenTo(BPoint(PenLocation().x -.5, PenLocation().y));
	Sync();
}