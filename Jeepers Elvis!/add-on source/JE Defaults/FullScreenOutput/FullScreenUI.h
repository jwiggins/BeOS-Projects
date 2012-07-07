/*
	FullScreenUI.h
	FullScreenUI class for FullScreen Output (a Jeepers Elvis! plugin)
	2000 John Wiggins
*/

#ifndef FULLSCREEN_UI_H
#define FULLSCREEN_UI_H

#include <interface/Alert.h>
#include <interface/View.h>
#include <interface/Button.h>
#include <interface/CheckBox.h>
#include <interface/Menu.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuField.h>
#include <interface/ListView.h>
#include <interface/ScrollView.h>
#include <interface/Rect.h>
#include <app/Message.h>
#include <posix/stdio.h>
#include <posix/string.h>

#include "VideoScreen.h"
#include "FullScreenOut.h"
#include "OverlayWindow.h"

enum {
	FS_TOGGLE_FULL_SCREEN = 'fstg',
	FS_TOGGLE_OVERLAY = 'fsuo',
	FS_SWITCH_RESOLUTION = 'swch'
};

class FullScreenUI : public BView {
public:
								FullScreenUI(BRect frame, const char *name,
								 struct fullscreen_output_private *data);
	
	virtual	void				AttachedToWindow();
	virtual	void				MessageReceived(BMessage *msg);
	virtual	void				GetPreferredSize(float *w, float *h);
	
private:
	
	fullscreen_output_private	*cookie; // VideoScreen pointer in here
	BButton						*toggleButton;
	BCheckBox					*olCheckBox;
	BMenuField					*resMenu;
	float						width, height;
	bool						useOverlay;
};

#endif