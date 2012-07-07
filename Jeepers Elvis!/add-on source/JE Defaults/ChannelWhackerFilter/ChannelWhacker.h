/*
	ChannelWhacker.h
	ChannelWhacker prototypes
	2000 John Wiggins
*/

#ifndef CHANNEL_WHACKER_H
#define CHANNEL_WHACKER_H

#include "JEPlugin.h"

void * instantiate_channel_whacker_filter(void *arg);
void channel_whacker_filter_filter(je_filter_plugin *cookie, BBitmap *frame);
void channel_whacker_filter_free(je_filter_plugin *cookie);
BView *channel_whacker_filter_getUI(je_filter_plugin *cookie);

#endif