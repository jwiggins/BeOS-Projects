/*
	MovieLooperSource.cpp
	The Movie Looper source plugin(a Jeepers Elvis plugin)
	2000 John Wiggins
*/

#include <interface/View.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <support/Locker.h>
#include <support/Autolock.h>
#include <kernel/OS.h>

//#include "JEPlugin.h"
#include "MovieLooper.h"
#include "VideoReader.h"
#include "MovieLooperUI.h"

// private data
struct movie_looper_private {
	BMessenger *graphPipe;
	BMessenger *monitorWin;
	VideoReader *vidReader;
	MovieLooperUI *uiView;
	BBitmap *frame;
	BLocker mutexLock;
	BLocker exitLock;
	bool exit;
	bool ready;
	bool stopped;
	int32 id;
};

// functions
void * movie_looper_instantiate(void *arg)
{
	je_source_args *args = (je_source_args *)arg;
	movie_looper_private *data = new movie_looper_private;
	je_source_plugin *plugin = new je_source_plugin;
	
	// fill in data
	data->graphPipe = args->graphics_pipeline;
	data->monitorWin = args->window;
	data->id = args->id;
	data->ready = false;
	data->exit = false;
	data->stopped = false;
	data->frame = NULL;
	// construct the VideoReader
	data->vidReader = new VideoReader;
	// and the MovieLooperUI
	BRect bounds(0.0, 0.0, 199.0, 99.0);
	data->uiView = new MovieLooperUI(bounds, "uiview",
							data->vidReader, &data->frame, &data->mutexLock);
	
	// fill in plugin
	plugin->data = data;
	plugin->start = movie_looper_start;
	plugin->stop = movie_looper_stop;
	plugin->run = movie_looper_run;
	plugin->exit = movie_looper_exit;
	plugin->free = movie_looper_free;
	plugin->lock = movie_looper_lock;
	plugin->unlock = movie_looper_unlock;
	plugin->getUI = movie_looper_getUI;
	plugin->getFrame = movie_looper_getFrame;
	plugin->outputSize = movie_looper_outputSize;
	plugin->nextFrameTime = movie_looper_nextFrameTime;
	
	return (void *)plugin;
}

void movie_looper_start(je_source_plugin *cookie)
{
	movie_looper_private *self = (movie_looper_private *)cookie->data;
	self->stopped = false;
}

void movie_looper_stop(je_source_plugin *cookie)
{
	movie_looper_private *self = (movie_looper_private *)cookie->data;
	self->stopped = true;
}

void movie_looper_run(je_source_plugin *cookie)
{
	movie_looper_private *self = (movie_looper_private *)cookie->data;
	int32 vidClip = -1;
	BMessage drawBitmap(JE_DRAW_BITMAP);
	bigtime_t snoozeTime;
	
	// ensure clean exit
	self->exitLock.Lock();
	
	while (1)
	{
		// sleep until our next frame
		snoozeTime = self->vidReader->NextFrameTime();
		
		// snoozeTime == 0 means there's no clip loaded
		if (snoozeTime == 0)
		{
			// no clip
			vidClip = -1;
			// go to bed
			snooze_until(system_time() + (100 * 1000), B_SYSTEM_TIMEBASE);
		}
		else if (!self->stopped)
		{
			// oh boy, clip change
			if (vidClip == -1 || vidClip != self->vidReader->CurrentClip())
			{
				BMessage msg;
				
				// framerate might be different
				msg.what = JE_FRAMERATE_CHANGED;
				msg.AddInt64("framerate", self->vidReader->ClipFramerate());
				msg.AddInt32("which", self->id);
				self->graphPipe->SendMessage(&msg);
				
				// frame size might be different
				msg.RemoveName("framerate");
				msg.what = JE_FRAME_SIZE_CHANGED;
				self->graphPipe->SendMessage(&msg);
				
				// vidClip is different
				vidClip = self->vidReader->CurrentClip();
			}
			
			// go to bed
			snooze_until(snoozeTime, B_SYSTEM_TIMEBASE);
			
			// lock
			if (self->mutexLock.Lock())
			{
				// lock vidReader
				if (self->vidReader->Lock())
				{
					// grab a frame
					self->vidReader->GetNextFrame(self->frame);
					// unlock vidReader
					self->vidReader->Unlock();
				}
				// unlock
				self->mutexLock.Unlock();
			}
			
			// tell your monitor window about the new frame
			drawBitmap.RemoveName("bitmap");
			drawBitmap.AddPointer("bitmap", self->frame);
			self->monitorWin->SendMessage(&drawBitmap);
			
			// we're ready now if we weren't already
			if (!self->ready)
			{
				BMessage msg(JE_SOURCE_READY);
				
				msg.AddInt32("which", self->id);
				self->graphPipe->SendMessage(&msg);
				self->ready = true;
			}
		}
		else // stopped == true
		{
			// never miss a chance to nap
			snooze(self->vidReader->ClipFramerate());
		}
		
		// check the exit flag	
		if (self->exit)
			break;
	}
	
	// ok, we're done
	self->exitLock.Unlock();
}

void movie_looper_exit(je_source_plugin *cookie)
{
	movie_looper_private *self = (movie_looper_private *)cookie->data;
	
	// set the exit flag
	self->exit = true;
	// block until run() has finished
	self->exitLock.Lock();
}

void movie_looper_free(je_source_plugin *cookie)
{
	movie_looper_private *self = (movie_looper_private *)cookie->data;
	
	// delete all the stuff in self
	delete self->graphPipe;
	delete self->monitorWin;
	if (self->frame != NULL)
		delete self->frame;
	delete self->vidReader;
	delete self->uiView;
	
	// then delete self
	delete self;
}

bool movie_looper_lock(je_source_plugin *cookie)
{
	movie_looper_private *self = (movie_looper_private *)cookie->data;
	
	return self->mutexLock.Lock();
}

void movie_looper_unlock(je_source_plugin *cookie)
{
	movie_looper_private *self = (movie_looper_private *)cookie->data;
	
	self->mutexLock.Unlock();
}

BView *movie_looper_getUI(je_source_plugin *cookie)
{
	movie_looper_private *self = (movie_looper_private *)cookie->data;
	
	return self->uiView;
}

BBitmap *movie_looper_getFrame(je_source_plugin *cookie)
{
	movie_looper_private *self = (movie_looper_private *)cookie->data;
	
	return self->frame;
}

BRect movie_looper_outputSize(je_source_plugin *cookie)
{
	movie_looper_private *self = (movie_looper_private *)cookie->data;
	float w, h;
	
	// wrap this in a lock
	self->vidReader->Lock();
	self->vidReader->GetClipDimensions(&w,&h);
	self->vidReader->Unlock();
	
	return BRect(0.0,0.0,w-1.0,h-1.0);
}

bigtime_t movie_looper_nextFrameTime(je_source_plugin *cookie)
{
	movie_looper_private *self = (movie_looper_private *)cookie->data;
	
	return self->vidReader->NextFrameTime();
}
