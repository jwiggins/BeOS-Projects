/*
	stub_filter.h
	Skeleton filter plugin for Jeepers Elvis!
	2000 John Wiggins
*/

#include <interface/View.h>
#include <interface/Bitmap.h>

#include "JEPlugin.h"

void * instantiate_stub_filter(void *arg);
void stub_filter_filter(je_filter_plugin *cookie, BBitmap *frame);
void stub_filter_free(je_filter_plugin *cookie);
BView *stub_filter_getui(je_filter_plugin *cookie);