/*
	FilterListView.cpp
	2000 John Wiggins
*/

#include "FilterListView.h"
#include "MainWindow.h"

FilterListView::FilterListView(BRect frame, const char *name, uint32 what)
: BListView(frame, name),
 highlightRect(Bounds())
{
	// adjust our highlight rectangle
	highlightRect.InsetBy(1.0,1.0);
	
	// our desired drop msg what constant
	dropWhat = what;
	
	// no selection yet
	currentSelection = -1;
	
	// no drag to indicate
	indicatorLine = 0.0;
	
	// we're not currently highlighted
	drawHighlight = false;
	
	// we're not dragging either
	isDragging = false;
	
	// where a dragged item will go
	moveToIndex = 0;
	
	// our highlight color
	highlightColor.red = 255;
	highlightColor.green = 0;
	highlightColor.blue = 0;
	highlightColor.alpha = 255;
}

FilterListView::~FilterListView()
{
	// we're in charge of freeing our items
	for (int32 i=0; i < CountItems(); i++)
	{
		BListItem *item = RemoveItem(0L);
		delete item;
	}
}

void FilterListView::MessageReceived(BMessage *msg)
{
	// check for our drop msg
	if (msg->what == dropWhat)
	{
		// pass it to our parent window
		msg->AddInt32("which", CountItems());
		((MainWindow *)Window())->PluginDropped(msg);
	}
	else // handle other messages
	{
		switch(msg->what)
		{
			default:
			{
				BListView::MessageReceived(msg);
				break;
			}
		}
	}
}

void FilterListView::Draw(BRect update)
{
	// let BListView take care of drawing the items
	BListView::Draw(update);
	
	// highlight if we need to
	if (drawHighlight)
	{
		SetHighColor(highlightColor);
		StrokeRect(highlightRect);
	}
	
	// an item is being dragged
	// draw a line where the item will go
	if (isDragging)
	{
		SetHighColor(highlightColor);
		StrokeLine(BPoint(update.left, indicatorLine), BPoint(update.right, indicatorLine));
	}
}

//void FilterListView::MouseDown(BPoint where)
//{
//	// let BListView do what it needs to
//	BListView::MouseDown(where);
//	
//	// set an event mask so we get events until the mouse button is released
//	SetMouseEventMask(B_POINTER_EVENTS);
//	// set our tracking flag
//	trackingMouse = true;
//	// cache where they clicked
//	mouseDownPt = where;
//}

void FilterListView::MouseUp(BPoint where)
{
	// let BListView do what it needs to
	BListView::MouseUp(where);
	
	// is it time to stop highlighting?
	if (drawHighlight)
	{
		// stop highlighting
		drawHighlight = false;
		// force redraw
		Invalidate(Bounds());
	}
	
	// check to see if we just ended a drag
	if (isDragging)
	{
		// no longer dragging
		isDragging = false;
		
		// is the mouse inside our Bounds() ?
		if (Bounds().Contains(where))
		{
			// ok, move the currently selected item to moveToIndex
			//printf("FilterListView::MoveItem(%d,%d);\n",currentSelection, moveToIndex);
			MoveItem(currentSelection, moveToIndex);
			// tell the MainWindow about it
			((MainWindow *)Window())->MoveFilterPlugin(currentSelection, moveToIndex);
			
			// change the current selection
			currentSelection = moveToIndex;
			// indicate the changed selection
			Select(moveToIndex);
			// erase the indicator
			Invalidate(BRect(Bounds().left,indicatorLine-1.0,Bounds().right,indicatorLine+1.0));
		}
	}
}

void FilterListView::MouseMoved(BPoint where, uint32 code, const BMessage *msg)
{
	//printf("FilterListView::MouseMoved()\n");
	// let BListView do what it needs to
	BListView::MouseMoved(where, code, msg);
	
	// check for our drag message
	if (msg != NULL && msg->what == dropWhat)
	{
		// if the mouse just entered
		if (code == B_ENTERED_VIEW)
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
		// if the mouse is leaving the view, then a filter is being removed
		
		if (code == B_EXITED_VIEW)
		{
			// tell main window the plugin is going away
			((MainWindow *)Window())->UnloadPlugin(JE_FILTER_PLUGIN, currentSelection);
			// get rid of its item
			RemoveFilterItem(currentSelection);
			// erase the indicator
			Invalidate(BRect(Bounds().left,indicatorLine-1.0,Bounds().right,indicatorLine+1.0));
			// no longer dragging
			isDragging = false;
		}
		else // still in the view
		{
			// draw some indication of where the item will go
			// if the mouse button is released
			int32 overIndex = IndexOf(where);
			BRect itemFrame;
			
			// validate overIndex
			if (overIndex < 0)
				overIndex = CountItems()-1;
			
			// get itemFrame
			itemFrame = ItemFrame(overIndex);
			
			// if the mouse is above the center line of the item
			if (where.y < (itemFrame.top + (itemFrame.Height()/2.0)))
			{
				if (overIndex == currentSelection)
					// set moveToIndex
					moveToIndex = overIndex;
				else if (overIndex > currentSelection)
					// set moveToIndex
					moveToIndex = overIndex - 1;
				else
					// set moveToIndex
					moveToIndex = overIndex;		
				// indicator goes here
				indicatorLine = itemFrame.top;
				
				//printf("moveToIndex = %d, overIndex = %d\n", moveToIndex, overIndex);
			}
			else // below the center line
			{
				if (overIndex == currentSelection)
					// set moveToIndex
					moveToIndex = overIndex;
				else if (overIndex < currentSelection)
					// set moveToIndex
					moveToIndex = overIndex + 1;
				else
					// set moveToIndex
					moveToIndex = overIndex;
				// indicator goes here
				indicatorLine = itemFrame.bottom;
					
				//printf("moveToIndex = %d, overIndex = %d\n", moveToIndex, overIndex);
			}	
			
			// push the bottom of the item frame down a little
			itemFrame.bottom += 1.0;
			// and the top up a little
			itemFrame.top -= 1.0;
			// invalidate
			Invalidate(itemFrame);
		}
	}
}

bool FilterListView::InitiateDrag(BPoint pt, int32 index, bool selected)
{
	//printf("FilterListView::InitiateDrag()\n");
	
	// if the selection changed
	if (index != currentSelection)
	{
		// make an (empty) unload msg
		BMessage msg(JE_UNLOAD_UI);
		// unload the UI for the previous item
		((MainWindow *)Window())->UnloadPluginUI(JE_FILTER_PLUGIN, currentSelection, msg);
		// and set the current selection
		currentSelection = index;
	}
	
	// start a drag & drop
	BMessage drag(JE_CONTAINER_VIEW_DND);
	BBitmap *dragMap;
	BView *dragView;
	BListItem *item = ItemAt(currentSelection);
	BRect itemFrame = ItemFrame(currentSelection);
	BPoint dragPoint;
	
	// determine where in the item the mouse is
	dragPoint.x = pt.x - itemFrame.left;
	dragPoint.y = pt.y - itemFrame.top;
	
	// offset to (0,0)
	itemFrame.OffsetTo(0.0,0.0);
	
	// make the bitmap
	dragMap = new BBitmap(itemFrame, B_RGB32, true);
	
	// make the view that will draw into the bitmap
	dragView = new BView(itemFrame, "view", B_FOLLOW_NONE,0);
	// and add it to the bitmap
	dragMap->AddChild(dragView);
	
	// now, draw the item
	if (dragMap->Lock())
	{
		// prepare the view
		dragView->SetDrawingMode(B_OP_ALPHA);
		dragView->SetHighColor(0, 0, 0, 128);
		dragView->SetLowColor(255, 255, 255, 128);
		dragView->SetBlendingMode(B_CONSTANT_ALPHA,B_ALPHA_COMPOSITE);
		dragView->StrokeRect(itemFrame, B_SOLID_LOW);
		// draw the item
		item->Deselect();
		item->DrawItem(dragView, itemFrame, true);
		item->Select();
		dragMap->Unlock();
	}
	
	// start the drag
	DragMessage(&drag, dragMap, B_OP_ALPHA, dragPoint);
	
	// we've started a drag
	isDragging = true;
	
	// set an event mask so we get events until the mouse button is released
	SetMouseEventMask(B_POINTER_EVENTS);
	
	return true;
}

void FilterListView::SelectionChanged()
{
	// make an (empty) unload msg
	BMessage msg(JE_UNLOAD_UI);
	// unload the UI for the previous item
	((MainWindow *)Window())->UnloadPluginUI(JE_FILTER_PLUGIN, currentSelection, msg);
	// and set the current selection
	currentSelection = CurrentSelection();
}

void FilterListView::SetUIButton(BButton *butt)
{
	uiButton = butt;
	
	if (uiButton != NULL)
	{
		// if there is at least one occupant
		if (CountItems() > 0)
			uiButton->SetEnabled(true);
		else
			uiButton->SetEnabled(false);
	}
}

void FilterListView::AddFilterItem(const char *label)
{
	// add the item
	AddItem(new BStringItem(label));
	
	if (uiButton != NULL)
	{
		// if this is our first item
		if (CountItems() == 1)
			uiButton->SetEnabled(true);
	}
}

void FilterListView::RemoveFilterItem(int32 index)
{
	// remove the item
	BListItem *item = RemoveItem(index);
	
	// clean up the item
	if (item != NULL)
		delete item;
	
	if (uiButton != NULL)
	{
		// if that was the last one
		if (CountItems() == 0)
			uiButton->SetEnabled(false);
	}
}
