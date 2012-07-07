/*
	CaptureCardSource.cpp
	The CaptureCard source plugin(a Jeepers Elvis plugin)
	2001 John Wiggins
*/

#include <interface/View.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <support/Locker.h>
#include <support/Autolock.h>
#include <kernel/OS.h>

//#include "JEPlugin.h"
#include "CaptureCardSource.h"

// private data
struct capture_card_private {
	BMessenger *graphPipe;
	BMessenger *monitorWin;
	BBitmap *frame;
	BLocker mutexLock;
	BLocker exitLock;
	bool exit;
	bool ready;
	bool stopped;
	int32 id;
};

// functions
void * capture_card_instantiate(void *arg)
{
}

void capture_card_start(je_source_plugin *cookie)
{
}

void capture_card_stop(je_source_plugin *cookie)
{
}

void capture_card_run(je_source_plugin *cookie)
{
}

void capture_card_exit(je_source_plugin *cookie)
{
}

void capture_card_free(je_source_plugin *cookie)
{
}

bool capture_card_lock(je_source_plugin *cookie)
{
}

void capture_card_unlock(je_source_plugin *cookie)
{
}

BView *capture_card_getUI(je_source_plugin *cookie)
{
}

BBitmap *capture_card_getFrame(je_source_plugin *cookie)
{
}

BRect capture_card_outputSize(je_source_plugin *cookie)
{
}

bigtime_t capture_card_nextFrameTime(je_source_plugin *cookie)
{
}
