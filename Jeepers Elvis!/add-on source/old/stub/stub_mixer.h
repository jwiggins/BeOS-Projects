/*
	stub_mixer.h
	Skeleton mixer plugin for Jeepers Elvis!
	2000 John Wiggins
*/

#include <interface/View.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>

#include "JEPlugin.h"

void * instantiate_stub_mixer(void *arg);
void stub_mixer_mix(je_mixer_plugin *cookie, BBitmap *src1, BBitmap *src2, BBitmap *dst);
void stub_mixer_free(je_mixer_plugin *cookie);
BRect stub_mixer_getoutputsize(je_mixer_plugin *cookie, BRect src1, BRect src2);
BView *stub_mixer_getui(je_mixer_plugin *cookie);