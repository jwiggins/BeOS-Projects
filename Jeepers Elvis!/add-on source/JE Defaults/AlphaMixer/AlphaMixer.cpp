/*
	AlphaMixer.cpp
	The Alpha Mixer mixer plugin (a Jeepers Elvis plugin)
	2000 John Wiggins
*/

#include <interface/View.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <support/Locker.h>
#include <kernel/OS.h>
#include <posix/stdio.h>

//#include "JEPlugin.h"
#include "AlphaMixer.h"
#include "SuperMix.h"
#include "AlphaMixerUI.h"

// private data
struct alpha_mixer_private {
	SuperMix *mix;
	AlphaMixerUI *uiView;
};

// function definitions
void * instantiate_alpha_mixer(void *arg)
{
	alpha_mixer_private *data = new alpha_mixer_private;
	je_mixer_plugin *self = new je_mixer_plugin;
	
	
	// fill in data
	data->mix = new SuperMix;
	data->uiView = new AlphaMixerUI(BRect(0.0,0.0,199.0,69.0), "ui", data->mix);
	
	//printf("instantiate_alpha_mixer() : uiview = %p\n", data->uiView);
	
	// our plugin's private data
	self->data = data;
	// and the function pointers
	self->mix = alpha_mixer_mix;
	self->free = alpha_mixer_free;
	self->getOutputSize = alpha_mixer_getOutputSize;
	self->getUI = alpha_mixer_getUI;
	
	// return
	return (void *)self;
}

void alpha_mixer_mix(je_mixer_plugin *cookie, BBitmap *src1, BBitmap *src2, BBitmap *dst)
{
	alpha_mixer_private *self = (alpha_mixer_private *)cookie->data;
	int32 backWhich = self->uiView->GetBackground();
	
	//printf("alpha_mixer_mix()\n");
	
	if (src1 != NULL && src2 != NULL)
	{
		if (backWhich == 0)
			self->mix->Mix(src1, src2, dst);
		else
			self->mix->Mix(src2, src1, dst);
	}
	else if (src1 != NULL)
	{
		self->mix->Mix(src1, NULL, dst);
	}
	else // src2 != NULL
	{
		self->mix->Mix(src2, NULL, dst);
	}
}

void alpha_mixer_free(je_mixer_plugin *cookie)
{
	alpha_mixer_private *self = (alpha_mixer_private *)cookie->data;
	
	//printf("alpha_mixer_free() : uiview = %p\n", self->uiView);
	
	delete self->uiView;
	delete self->mix;
	delete self;
}

BRect alpha_mixer_getOutputSize(je_mixer_plugin *cookie, BRect src1, BRect src2)
{
	alpha_mixer_private *self = (alpha_mixer_private *)cookie->data;
	BRect ret;
	
	// check rect validity first!!!
	// at least 1 is guaranteed to be valid
	if (src1.IsValid() && src2.IsValid())
	{
		if (src1.Width() > src2.Width())
		{
			// src1 is wider
			ret = src1;
			// set the scale for the mixer
			self->mix->SetScale(src2, src1);
		}
		else if (src1.Width() < src2.Width())
		{
			// src2 is wider
			ret = src2;
			// set the scale for the mixer
			self->mix->SetScale(src1, src2);
		}
		else if (src1.Height() > src2.Height())
		{
			// widths are equal, but src1 is taller
			ret = src1;
			// set the scale for the mixer
			self->mix->SetScale(src2, src1);
		}
		else
		{
			// widths are equal, src2 is taller or heights are equal
			ret = src2;
			// set the scale for the mixer
			self->mix->SetScale(src1, src2);
		}
	}
	else if (src1.IsValid())
	{
		// set the return value
		ret = src1;
		// set the scale for the mixer
		// 1.0 in this case (src == dst)
		self->mix->SetScale(ret, ret);
	}
	else // src2.IsValid()
	{
		// set the return value
		ret = src2;
		// set the scale for the mixer
		// 1.0 in this case (src == dst)
		self->mix->SetScale(ret, ret);
	}
	
	return ret;
}

BView *alpha_mixer_getUI(je_mixer_plugin *cookie)
{
	alpha_mixer_private *self = (alpha_mixer_private *)cookie->data;
	
	return self->uiView;
}
