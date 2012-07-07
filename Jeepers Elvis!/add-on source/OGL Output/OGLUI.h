/*
	OGLUI.h
	OGLUI class for OGL Output (a Jeepers Elvis! plugin)
	2002 John Wiggins
*/

#ifndef OGL_UI_H
#define OGL_UI_H

#include <interface/Alert.h>
#include <interface/View.h>
#include <interface/Button.h>
#include <interface/Rect.h>
#include <app/Message.h>
#include <posix/stdio.h>
#include <posix/string.h>

#include "OGLWindow.h"

class OGLUI : public BView {
public:
								OGLUI(BRect frame, const char *name, OGLWindow *win);
	
	virtual	void				AttachedToWindow();
	virtual	void				MessageReceived(BMessage *msg);
	virtual	void				GetPreferredSize(float *w, float *h);
	
private:
	
	OGLWindow					*vidWin;
	BButton						*toggleButton;
	float						width, height;
};

#endif