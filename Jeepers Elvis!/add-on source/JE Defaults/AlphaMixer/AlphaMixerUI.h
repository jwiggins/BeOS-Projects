/*
	AlphaMixerUI.h
	AlphaMixerUI class for Alpha Mixer mixer plugin (a Jeepers Elvis plugin)
	2000 John Wiggins
*/

#ifndef ALPHA_MIXER_UI_H
#define ALPHA_MIXER_UI_H

#include <interface/View.h>
#include <interface/Rect.h>
#include <interface/RadioButton.h>
#include <interface/StringView.h>
#include <interface/Slider.h>
#include <app/Message.h>
#include <posix/stdio.h>

#include "SuperMix.h"

enum {
	AM_SET_BACKGROUND = 'setb',
	AM_SET_ALPHA = 'seta'
};

class AlphaMixerUI : public BView {
public:
							AlphaMixerUI(BRect frame, const char *name, SuperMix *mix);
							~AlphaMixerUI();
	
	virtual	void			GetPreferredSize(float *w, float *h);
	virtual	void			AttachedToWindow();
	virtual	void			DetachedFromWindow();
	virtual	void			MessageReceived(BMessage *msg);
	
	int32					GetBackground() const;
private:
	
	class FUCKING_BSlider : public BSlider {
	public:
							FUCKING_BSlider(BRect frame, const char *name,  SuperMix *mix);
		
		virtual	void 		SetValue(int32 v);
	private:
		SuperMix			*mixer;
	};
	SuperMix				*theMixer;
	BRadioButton			*radio1, *radio2;
	FUCKING_BSlider			*alphaSlider;
	
	int32					backWhich;
	float					width, height;
};

#endif