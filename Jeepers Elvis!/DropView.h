/*
	DropView.h
	DropView class declaration for Jeepers Elvis!
	2000 John Wiggins
*/

#ifndef DROP_VIEW_H
#define DROP_VIEW_H

#include <app/Message.h>
#include <interface/View.h>
#include <interface/Window.h>
#include <interface/Button.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <interface/Point.h>
#include <posix/math.h>

#include "MessageConstants.h"

class DropView : public BView {
public:
						DropView(BRect frame, const char *name, BBitmap *icon, uint32 what, int32 type, int32 _id);
						~DropView();
	
	virtual	void		MessageReceived(BMessage *msg);
	virtual	void		Draw(BRect update);
	virtual	void		MouseDown(BPoint where);
	virtual	void		MouseUp(BPoint where);
	virtual	void		MouseMoved(BPoint where, uint32 code, const BMessage *msg);
	
	void				ToggleOccupant(bool flag);
	void				SetUIButton(BButton *butt);

private:
	void				MakeDarkIcon(BBitmap *icon);
	
	BBitmap				*iconBitmap, *iconBitmapDark;
	BButton				*uiButton;
	BRect				highlightRect;
	BPoint				mouseDownPt;
	uint32				dropWhat;
	int32				id;
	int32				piType;
	bool				drawHighlight;
	bool				hasOccupant;
	bool				occupantLoading;
	bool				trackingMouse;
	bool				isDragging;
	rgb_color			highlightColor;
};

#endif