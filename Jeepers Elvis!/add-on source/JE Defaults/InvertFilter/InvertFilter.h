/*
	InvertFilter.h
	InvertFilter prototypes
	2000 John Wiggins
*/

#ifndef INVERT_FILTER_H
#define INVERT_FILTER_H

#include "JEPlugin.h"

void * instantiate_invert_filter(void *arg);
void invert_filter_filter(je_filter_plugin *cookie, BBitmap *frame);
void invert_filter_free(je_filter_plugin *cookie);
BView *invert_filter_getUI(je_filter_plugin *cookie);

#endif