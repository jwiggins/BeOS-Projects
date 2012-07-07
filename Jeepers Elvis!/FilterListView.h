/*
	FilterListView.h
	FilterListView class declaration for Jeepers Elvis!
	2000 John Wiggins
*/

#ifndef FILTER_LIST_VIEW_H
#define FILTER_LIST_VIEW_H

#include <app/Message.h>
#include <interface/ListView.h>
#include <interface/ListItem.h>
#include <interface/Window.h>
#include <interface/View.h>
#include <interface/Bitmap.h>
#include <interface/Button.h>
#include <interface/Rect.h>
#include <interface/Point.h>

#include "MessageConstants.h"
#include "JEPlugin.h"

class FilterListView : public BListView {
public:
						FilterListView(BRect frame, const char *name, uint32 what);
						~FilterListView();
	
	virtual	void		MessageReceived(BMessage *msg);
	virtual	void		Draw(BRect update);
//	virtual	void		MouseDown(BPoint where);
	virtual	void		MouseUp(BPoint where);
	virtual	void		MouseMoved(BPoint where, uint32 code, const BMessage *msg);
	virtual	bool		InitiateDrag(BPoint pt, int32 index, bool selected);
	virtual	void		SelectionChanged();
	
	void				SetUIButton(BButton *butt);
	void				AddFilterItem(const char *label);
	void				RemoveFilterItem(int32 index);

private:
//	class FilterListItem : public BStringItem {
//	public:
//						FilterListItem();
//						~FilterListItem();
//	private:
//	};
	BRect				highlightRect;
	BButton				*uiButton;
	uint32				dropWhat;
	int32				moveToIndex;
	int32				currentSelection;
	float				indicatorLine;
	bool				drawHighlight;
	bool				isDragging;
	rgb_color			highlightColor;
};

#endif