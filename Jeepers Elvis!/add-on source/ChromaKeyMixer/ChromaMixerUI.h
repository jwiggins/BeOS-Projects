/*
	ChromaMixerUI.h
	ChromaMixerUI class for Chroma Key Mixer mixer plugin (a Jeepers Elvis plugin)
	2000 John Wiggins
*/

#ifndef CHROMA_MIXER_UI_H
#define CHROMA_MIXER_UI_H

#include <interface/View.h>
#include <interface/Rect.h>
#include <interface/RadioButton.h>
#include <interface/StringView.h>
#include <interface/ColorControl.h>
#include <app/Message.h>
#include <posix/stdio.h>

#include "ChromaMix.h"

enum {
	CM_SET_BACKGROUND = 'setb',
	CM_SET_KEY = 'setk'
};

class ChromaMixerUI : public BView {
public:
							ChromaMixerUI(BRect frame, const char *name, ChromaMix *mix);
							~ChromaMixerUI();
	
	virtual	void			GetPreferredSize(float *w, float *h);
	virtual	void			AttachedToWindow();
	virtual	void			DetachedFromWindow();
	virtual	void			MessageReceived(BMessage *msg);
	
	int32					GetBackground() const;
private:
	ChromaMix				*theMixer;
	BRadioButton			*radio1, *radio2;
	BColorControl			*keyColor;
		
	int32					backWhich;
	float					width, height;
};

#endif