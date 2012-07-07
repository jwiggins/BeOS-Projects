/*
	OverlayUI.cpp
	2002 John Wiggins
*/

#include "OverlayUI.h"

OverlayUI::OverlayUI(BRect frame, const char *name, OverlayWindow *win)
: BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	BRect rect;
	BMessage *msg;
	
	// view color
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	// cache width and height
	width = frame.Width();
	height = frame.Height();
		
	// a nice pretty button
	rect.Set(width/5., height/5., width-(width/5.), height-(height/5.));
	msg = new BMessage(OO_VISIBILITY_TOGGLE);
	AddChild(toggleButton = new BButton(rect, "toggle", "Start", msg));
	
	// the fullscreen window we control
	vidWin = win;
}

void OverlayUI::AttachedToWindow()
{
	toggleButton->SetTarget(vidWin);
}

void OverlayUI::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}

void OverlayUI::GetPreferredSize(float *w, float *h)
{
	*w = width;
	*h = height;
}
