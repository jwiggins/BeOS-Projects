/*
	stub_output.h
	Skeleton output plugin for Jeepers Elvis!
	2000 John Wiggins
*/

#include <interface/View.h>
#include <interface/Bitmap.h>

#include "JEPlugin.h"

void * instantiate_stub_output(void *arg);
void stub_output_output(je_output_plugin *cookie, BBitmap *frame);
void stub_output_free(je_output_plugin *cookie);
BView *stub_output_getui(je_output_plugin *cookie);