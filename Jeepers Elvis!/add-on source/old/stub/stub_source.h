/*
	stub_source.h
	Skeleton source plugin for Jeepers Elvis!
	2000 John Wiggins
*/

#include <interface/View.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <app/Messenger.h>
#include <kernel/OS.h>
#include <support/SupportDefs.h>

#include "JEPlugin.h"

void * instantiate_stub_source(void *arg);
void stub_source_start(je_source_plugin *cookie);
void stub_source_stop(je_source_plugin *cookie);
void stub_source_run(je_source_plugin *cookie);
void stub_source_exit(je_source_plugin *cookie);
void stub_source_free(je_source_plugin *cookie);
bool stub_source_lock(je_source_plugin *cookie);
void stub_source_unlock(je_source_plugin *cookie);
BView *stub_source_getui(je_source_plugin *cookie);
BBitmap *stub_source_getframe(je_source_plugin *cookie);
BRect stub_source_outputsize(je_source_plugin *cookie);
bigtime_t stub_source_nextframetime(je_source_plugin *cookie);