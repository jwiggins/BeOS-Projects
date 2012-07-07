/*
	ChannelWhacker.cpp
	ChannelWhacker filter plugin for Jeepers Elvis!
	2000 John Wiggins
*/

#include <interface/Bitmap.h>

//#include "JEPlugin.h"
#include "ChannelWhacker.h"
#include "ChannelWhackerUI.h"
#include "WhackerFilt.h"


// plugin data
struct channel_whacker_filter_private {
	ChannelWhackerUI *uiView;
	WhackerFilt *filt;
};

// function defs
void * instantiate_channel_whacker_filter(void *arg)
{
	channel_whacker_filter_private *data = new channel_whacker_filter_private;
	je_filter_plugin *me = new je_filter_plugin;
	
	// fill in data
	data->filt = new WhackerFilt;
	data->uiView = new ChannelWhackerUI(BRect(0.0,0.0,199.0,149.0), "ui", data->filt);

	// fill in the plugin struct
	me->data = data;
	me->filter = channel_whacker_filter_filter;
	me->free = channel_whacker_filter_free;
	me->getUI = channel_whacker_filter_getUI;
	
	return (void *)me;
}

void channel_whacker_filter_filter(je_filter_plugin *cookie, BBitmap *frame)
{
	channel_whacker_filter_private *self = (channel_whacker_filter_private *)cookie->data;
	
	// filter
	self->filt->Filter(frame);
}

void channel_whacker_filter_free(je_filter_plugin *cookie)
{
	channel_whacker_filter_private *self = (channel_whacker_filter_private *)cookie->data;
	
	delete self->uiView;
	delete self->filt;
	delete self;
}

BView *channel_whacker_filter_getUI(je_filter_plugin *cookie)
{
	channel_whacker_filter_private *self = (channel_whacker_filter_private *)cookie->data;
	
	return self->uiView;
}
