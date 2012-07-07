/*
	DropView.cpp
	2000 John Wiggins
*/

#include "DropView.h"
#include "MainWindow.h"

DropView::DropView(BRect frame, const char *name, BBitmap *icon, uint32 what, int32 type, int32 _id)
: BView(frame, name, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW),
 highlightRect(Bounds()), mouseDownPt(0.0f,0.0f)
{
	// construct and init the dark icon
	MakeDarkIcon(icon);
	// our icon bitmap. We assume responsibility for it
	iconBitmap = icon;
	
	// setup our highlight rectangle
	highlightRect.InsetBy(1.0,1.0);
	
	// our id
	id = _id;
	
	// our plugin's type
	piType = type;
	
	// the button that asks for our plugin's UI
	uiButton = NULL;
	
	// we aren't occupied
	hasOccupant = false;
	
	// we aren't about to be occupied
	occupantLoading = false;
	
	// our desired drop msg what constant
	dropWhat = what;
	
	// we aren't currently highlighted
	drawHighlight = false;
	
	// we aren't tracking the mouse
	trackingMouse = false;
	
	// and we our occupant isn't being dragged
	isDragging = false;
	
	// our highlight color
	highlightColor.red = 255;
	highlightColor.green = 0;
	highlightColor.blue = 0;
	highlightColor.alpha = 255;
	
	// set the view color
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

DropView::~DropView()
{
	delete iconBitmap;
	if (iconBitmapDark != NULL)
		delete iconBitmapDark;
}

void DropView::MessageReceived(BMessage *msg)
{
	// check for our drop message
	if (msg->what == dropWhat)
	{
		//printf("DropView::MessageReceived(dropWhat)\n");
		// if we don't already have an occupant
		// then go ahead and post it to our parent window
		if (!hasOccupant && !occupantLoading)
		{
			msg->AddInt32("which", id);
			((MainWindow *)Window())->PluginDropped(msg);
			
			// now, we wait
			occupantLoading = true;
		}
	}
	else // handle other messages
	{
		switch(msg->what)
		{
			default:
			{
				BView::MessageReceived(msg);
				break;
			}
		}
	}
}

void DropView::Draw(BRect update)
{
	BRect bounds = Bounds();
	//draw our bevel
	// top and left edges
	SetHighColor(150,150,150,255); // dark grey
	StrokeLine(bounds.LeftTop(), bounds.RightTop());
	StrokeLine(bounds.LeftTop(), bounds.LeftBottom());
	// bottom and right edges
	SetHighColor(250,250,250,255); // lightest grey
	StrokeLine(bounds.RightBottom(), BPoint(bounds.right, bounds.top - 1.0));
	StrokeLine(bounds.RightBottom(), BPoint(bounds.left - 1.0, bounds.bottom));
	
	// draw our highlight, or don't
	if (drawHighlight)
	{
		SetHighColor(highlightColor);
		StrokeRect(highlightRect);
	}
	
	// draw our icon
	if (hasOccupant)
	{
		SetDrawingMode(B_OP_OVER);
		DrawBitmap(iconBitmap, BPoint(3.0, 3.0));
	}
	else
		DrawBitmap(iconBitmapDark, BPoint(3.0, 3.0));
}

void DropView::MouseDown(BPoint where)
{
	// if they clicked on the image (pretty likely)
	// AND we have an occupant
	if (highlightRect.Contains(where) && hasOccupant)
	{
		// set an event mask so we get events until the mouse button is released
		SetMouseEventMask(B_POINTER_EVENTS);
		// set our tracking flag
		trackingMouse = true;
		// cache where they clicked
		mouseDownPt = where;
	}
}

void DropView::MouseUp(BPoint where)
{
	// no longer traking
	trackingMouse = false;
	// or dragging
	if (isDragging)
		isDragging = false;
	// or highlighting
	if (drawHighlight)
	{
		// stop highlighting
		drawHighlight = false;
		// force redraw
		Invalidate(Bounds());
	}
}

void DropView::MouseMoved(BPoint where, uint32 code, const BMessage *msg)
{
	// check for our drag message
	if (msg != NULL && msg->what == dropWhat)
	{
		// if we ain't occupied, aren't about to be and
		// the mouse just entered
		if (!hasOccupant && !occupantLoading && code == B_ENTERED_VIEW)
		{
			// you can drop that here!
			// give the user some visual feedback
			drawHighlight = true;
			// force redraw
			Invalidate(Bounds());
		}
		else if (code == B_EXITED_VIEW)
		{
			// it left, stop highlighting
			drawHighlight = false;
			// force redraw
			Invalidate(Bounds());
		}
	}
	else if (msg != NULL && msg->what == JE_CONTAINER_VIEW_DND && isDragging)
	{
		// this is a drag initiated by us
		// if the mouse is leaving the view, then our occupant
		// is being removed
		
		if (code == B_EXITED_VIEW)
		{
			// tell the main window our plugin is gone
			((MainWindow *)Window())->UnloadPlugin(piType,id);
			ToggleOccupant(false);
		}
	}
	else if (trackingMouse)
	{
		// we're tracking the mouse after it clicked on us
		float dx = where.x - mouseDownPt.x; // delta x
		float dy = where.y - mouseDownPt.y; // delta y
		float dp = (float)sqrt((dx*dx)+(dy*dy)); // delta magnitude
		
		// if the mouse has moved futher than 5 pixels in any direction
		if (dp > 5.0)
		{
			// initiate a drag
			BMessage drag(JE_CONTAINER_VIEW_DND);
			DragMessage(&drag, new BBitmap(iconBitmap), B_OP_OVER, BPoint(where.x-2.0, where.y-2.0));
			
			// stop tracking the mouse
			trackingMouse = false;
			// our occupant is being dragged
			isDragging = true;
		}
	}
}

void DropView::ToggleOccupant(bool flag)
{
	// set the occupant flag
	hasOccupant = flag;
	
	// if we just got a plugin
	if (flag)
	{
		// unset the loading flag
		occupantLoading = false;
		// enable the ui button
		if (uiButton != NULL)
			uiButton->SetEnabled(true);
	}
	else
	{
		// disable the ui button
		if (uiButton != NULL)
			uiButton->SetEnabled(false);
	}
	// force a redraw
	Invalidate(Bounds());
}

void DropView::SetUIButton(BButton *butt)
{
	uiButton = butt;
	
	// disable if there isn't an occupant
	if (!hasOccupant)
		uiButton->SetEnabled(false);
	else
		uiButton->SetEnabled(true);
}

void DropView::MakeDarkIcon(BBitmap *icon)
{
	// this is a little wompedy
	// we first construct the normal icon with an offscreen window
	// then we draw a darkened version of the icon into it
	// then we copy it into a normal bitmap and destroy the
	// bitmap with the offscreen
	
	iconBitmap = new BBitmap(icon, true); // copy the icon
	
	// check its validity
	if (iconBitmap->IsValid())
	{
		// add a view
		iconBitmap->AddChild(new BView(iconBitmap->Bounds(), "draw", B_FOLLOW_NONE, 0));
		
		// and darken
		if (iconBitmap->Lock())
		{
			// set the drawing mode
			iconBitmap->ChildAt(0)->SetDrawingMode(B_OP_BLEND);
			// set the high color
			iconBitmap->ChildAt(0)->SetHighColor(192,192,192,255);
			// draw
			iconBitmap->ChildAt(0)->FillRect(iconBitmap->Bounds());
			iconBitmap->ChildAt(0)->Sync();
			iconBitmap->Unlock();
		}
	}
	
	// now copy into a normal bitmap
	iconBitmapDark = new BBitmap(iconBitmap, false);
	if (!iconBitmapDark->IsValid())
	{
		delete iconBitmapDark;
		iconBitmapDark = NULL;
	}
	
	// and get rid of the bitmap with an offscreen
	delete iconBitmap;
	iconBitmap = NULL;
}
