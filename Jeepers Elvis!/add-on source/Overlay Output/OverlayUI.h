/*
	OverlayUI.h
	OverlayUI class for Overlay Output (a Jeepers Elvis! plugin)
	2002 John Wiggins
*/

#ifndef OVERLAY_UI_H
#define OVERLAY_UI_H

#include <interface/Alert.h>
#include <interface/View.h>
#include <interface/Button.h>
#include <interface/Rect.h>
#include <app/Message.h>
#include <posix/stdio.h>
#include <posix/string.h>

#include "OverlayWindow.h"

class OverlayUI : public BView {
public:
								OverlayUI(BRect frame, const char *name,
									OverlayWindow *win);
	
	virtual	void				AttachedToWindow();
	virtual	void				MessageReceived(BMessage *msg);
	virtual	void				GetPreferredSize(float *w, float *h);
	
private:
	
	OverlayWindow				*vidWin;
	BButton						*toggleButton;
	float						width, height;
};

#endif