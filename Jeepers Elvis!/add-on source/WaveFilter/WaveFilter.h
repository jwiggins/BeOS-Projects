/*
	WaveFilter.h
	WaveFilter prototypes
	2002 John Wiggins
*/

#ifndef WAVE_FILTER_H
#define WAVE_FILTER_H

#include "JEPlugin.h"

void * instantiate_wave_filter(void *arg);
void wave_filter_filter(je_filter_plugin *cookie, BBitmap *frame);
void wave_filter_free(je_filter_plugin *cookie);
BView *wave_filter_getUI(je_filter_plugin *cookie);

#endif