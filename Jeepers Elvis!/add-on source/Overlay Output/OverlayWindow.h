/*
	OverlayWindow.h
	2002 John Wiggins
*/

#ifndef OVERLAY_WINDOW_H
#define OVERLAY_WINDOW_H

#include <app/Application.h>
#include <app/Message.h>
#include <interface/Window.h>
#include <interface/View.h>
#include <interface/Rect.h>
#include <interface/Bitmap.h>
#include <interface/Screen.h>
#include <posix/string.h>
#include <posix/stdio.h>

enum {
	OO_VISIBILITY_TOGGLE = 'oovt',
};

class OverlayView : public BView {
public:
						OverlayView(BRect frame);
						~OverlayView();
						
	virtual	void		Draw(BRect update);
	virtual void		KeyDown(const char *bytes, int32 numBytes);
	void				SetBitmap(BBitmap *bmp);
	void				UpdateBits(BBitmap *bmp);

private:

	BBitmap				*bitmap;
	BRect				bmpBounds;
	bool				usingOverlay;
	color_space			cSpace;
};

class OverlayWindow : public BWindow {
public:
						OverlayWindow(BRect frame, OverlayView **outView);
	
	virtual	bool		QuitRequested();
	virtual	void		MessageReceived(BMessage *msg);
private:
	
	OverlayView			*view;
};

#endif