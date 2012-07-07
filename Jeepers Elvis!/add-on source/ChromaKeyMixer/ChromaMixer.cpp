/*
	ChromaMixer.cpp
	The Chroma Key Mixer mixer plugin (a Jeepers Elvis plugin)
	2000 John Wiggins
*/

#include <interface/View.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <support/Locker.h>
#include <kernel/OS.h>
#include <posix/stdio.h>

#include "JEPlugin.h"
#include "ChromaMix.h"
#include "ChromaMixerUI.h"

// private data
struct chroma_mixer_private {
	ChromaMix *mix;
	ChromaMixerUI *uiView;
};

// function protos
void * instantiate_chroma_mixer(void *arg);
void chroma_mixer_mix(je_mixer_plugin *cookie, BBitmap *src1, BBitmap *src2, BBitmap *dst);
void chroma_mixer_free(je_mixer_plugin *cookie);
BRect chroma_mixer_getOutputSize(je_mixer_plugin *cookie, BRect src1, BRect src2);
BView *chroma_mixer_getUI(je_mixer_plugin *cookie);

// this plugin
je_plugin this_plugin = {JE_MIXER_PLUGIN, "Chroma Key Mixer", instantiate_chroma_mixer};

// the "plugins" in this add-on
static je_plugin *plugins[] = {
	&this_plugin,
	NULL
};

// the plugin's exported symbol
extern "C" _EXPORT je_plugin **get_plugins()
{
	return plugins;
}

// function definitions
void * instantiate_chroma_mixer(void *arg)
{
	chroma_mixer_private *data = new chroma_mixer_private;
	je_mixer_plugin *self = new je_mixer_plugin;
	
	
	// fill in data
	data->mix = new ChromaMix;
	data->uiView = new ChromaMixerUI(BRect(0.0,0.0,374.0,69.0), "ui", data->mix);
		
	// our plugin's private data
	self->data = data;
	// and the function pointers
	self->mix = chroma_mixer_mix;
	self->free = chroma_mixer_free;
	self->getOutputSize = chroma_mixer_getOutputSize;
	self->getUI = chroma_mixer_getUI;
	
	// return
	return (void *)self;
}

void chroma_mixer_mix(je_mixer_plugin *cookie, BBitmap *src1, BBitmap *src2, BBitmap *dst)
{
	chroma_mixer_private *self = (chroma_mixer_private *)cookie->data;
	int32 backWhich = self->uiView->GetBackground();
		
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

void chroma_mixer_free(je_mixer_plugin *cookie)
{
	chroma_mixer_private *self = (chroma_mixer_private *)cookie->data;
	
	delete self->uiView;
	delete self->mix;
	delete self;
}

BRect chroma_mixer_getOutputSize(je_mixer_plugin *cookie, BRect src1, BRect src2)
{
	chroma_mixer_private *self = (chroma_mixer_private *)cookie->data;
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

BView *chroma_mixer_getUI(je_mixer_plugin *cookie)
{
	chroma_mixer_private *self = (chroma_mixer_private *)cookie->data;
	
	return self->uiView;
}
