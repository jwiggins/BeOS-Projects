/*
	ExpMixerUI.h
	ExpMixerUI class for Experimental Mixer mixer plugin (a Jeepers Elvis plugin)
	2000 John Wiggins
*/

#ifndef CHROMA_MIXER_UI_H
#define CHROMA_MIXER_UI_H

#include <interface/View.h>
#include <interface/Rect.h>
#include <interface/RadioButton.h>
#include <interface/MenuField.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <interface/StringView.h>
#include <app/Message.h>
#include <posix/stdio.h>

#include "ExpMix.h"

enum {
	EM_SET_BACKGROUND = 'setb',
	EM_SET_OP = 'seto'
};

class ExpMixerUI : public BView {
public:
							ExpMixerUI(BRect frame, const char *name, ExpMix *mix);
							~ExpMixerUI();
	
	virtual	void			GetPreferredSize(float *w, float *h);
	virtual	void			AttachedToWindow();
	virtual	void			DetachedFromWindow();
	virtual	void			MessageReceived(BMessage *msg);
	
	int32					GetBackground() const;
private:
	ExpMix					*theMixer;
	BRadioButton			*radio1, *radio2;
	BMenuField				*opMenu;
		
	int32					backWhich;
	float					width, height;
};

#endif