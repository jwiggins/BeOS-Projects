/*
	VideoWindow.h
	VideoWindow class declaration for Jeepers Elvis!
	2000 John Wiggins
*/

#ifndef VIDEO_WINDOW_H
#define VIDEO_WINDOW_H

#include <app/Application.h>
#include <app/Message.h>
#include <interface/Window.h>
#include <interface/View.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <interface/Region.h>
#include <posix/stdio.h>

#include "MessageConstants.h"

class VideoWindow : public BWindow {
public:
							VideoWindow(BRect frame, const char *name, bool small);
	
	virtual	void			MessageReceived(BMessage *msg);
	virtual	bool			QuitRequested();
private:
	// the VideoView class
	class VideoView : public BView {
	public:
							VideoView(BRect frame, bool small);
							~VideoView();
		
		virtual	void		Draw(BRect update);
		void				UpdateFrame(BBitmap *frame);
		void				DrawColorBars();
	private:
	
		BBitmap				*vidFrame;
		BRect				vidBounds;
		BRegion				nonVidBounds;
		bool				isSmall;
	};
	
	VideoView				*vidView;
	bool					isSmall;
};

#endif