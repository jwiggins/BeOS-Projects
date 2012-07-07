/*
	ChannelWhackerUI.h
	ChannelWhackerUI class for ChannelWhacker filter plugin
	2000 John Wiggins
*/

#ifndef CHANNEL_WHACKER_UI_H
#define CHANNEL_WHACKER_UI_H

#include <interface/View.h>
#include <interface/StringView.h>
#include <interface/Slider.h>
#include <interface/CheckBox.h>
#include <interface/Rect.h>
#include <interface/Window.h>
#include <app/Message.h>
#include <posix/stdio.h>

#include "WhackerFilt.h"

enum {
//	CW_CHANNEL_ADD_REMOVE = 'chan'
	CW_CHANNEL_CHANGE = 'chch'
};

class ChannelWhackerUI : public BView {
public:
						ChannelWhackerUI(BRect frame, const char *name, WhackerFilt *filt);
	
	virtual	void		MessageReceived(BMessage *msg);
	virtual	void		AttachedToWindow();
	virtual	void		GetPreferredSize(float *w, float *h);

private:
	float				width, height;
//	BCheckBox			*redCheck, *greenCheck, *blueCheck;
	BSlider				*redSlide, *greenSlide, *blueSlide;
	WhackerFilt			*filter;
};

#endif