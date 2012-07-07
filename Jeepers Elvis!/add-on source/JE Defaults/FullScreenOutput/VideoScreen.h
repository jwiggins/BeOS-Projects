/*
	VideoScreen.h
	VideoScreen class for FullScreen Output (a Jeepers Elvis! plugin)
	2000 John Wiggins
*/

#ifndef VIDEO_SCREEN_H
#define VIDEO_SCREEN_H

#include <game/WindowScreen.h>
#include <interface/Bitmap.h>
#include <interface/InterfaceDefs.h>
#include <app/Message.h>
#include <kernel/OS.h>
#include <support/Locker.h>
#include <posix/stdio.h>

#include "InputFilter.h"

class VideoScreen : public BWindowScreen {
public:
							VideoScreen(uint32 space, status_t *ret);
	
	virtual	bool			QuitRequested();
	virtual	void			ScreenConnected(bool active);
	virtual	void			MessageReceived(BMessage *msg);
	
	void					Draw(BBitmap *bmp);
private:

	BLocker					accessLock;
	int32					disHeight, disWidth;
	int32					frmHeight, frmWidth;
	float					xScale, yScale;
	uint32*					frameBuffer;
	uint16					lineLength;
	volatile bool			connected;
	bool					disSizeInvalid;
};

#endif