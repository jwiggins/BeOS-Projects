/*
	stub_source.cpp
	2000 John Wiggins
*/

#include "stub_source.h"
#include "DebugView.h"

struct source_private {
	BView *uiView;
	BBitmap *frame;
	BRect outSize;
	bigtime_t nextFrame;
	BMessenger *graphics_pipeline;
	BMessenger *window;
	int32 id;
	bool exit;
	sem_id	exitSem;
};

void * instantiate_stub_source(void *arg)
{
	je_source_plugin *me = new je_source_plugin;
	source_private *data = new source_private;
	
	// fill in data
	data->uiView = new DebugView(BRect(0.0,0.0,74.0,49.0), "source", B_FOLLOW_ALL, B_WILL_DRAW);
	data->frame = new BBitmap(BRect(0.0,0.0,159.0,119.0), B_RGB32);
	data->outSize.Set(0.0,0.0,159.0,119.0);
	data->nextFrame = system_time() + 33000;
	data->graphics_pipeline = ((je_source_args *)arg)->graphics_pipeline;
	data->window = ((je_source_args *)arg)->window;
	data->id = ((je_source_args *)arg)->id;
	data->exit = false;
	data->exitSem = create_sem(0, "source exit sem");
	
	// fill in me
	me->start = stub_source_start;
	me->stop = stub_source_stop;
	me->run = stub_source_run;
	me->exit = stub_source_exit;
	me->free = stub_source_free;
	me->lock = stub_source_lock;
	me->unlock = stub_source_unlock;
	me->getUI = stub_source_getui;
	me->getFrame = stub_source_getframe;
	me->outputSize = stub_source_outputsize;
	me->nextFrameTime = stub_source_nextframetime;
	me->data = (void *)data;
	
	// return me
	return (void *)me;
}

void stub_source_start(je_source_plugin *cookie)
{
}

void stub_source_stop(je_source_plugin *cookie)
{
}

void stub_source_run(je_source_plugin *cookie)
{
	source_private *data = (source_private *)cookie->data;
	BMessage drawBitmap(JE_DRAW_BITMAP);
	BMessage msg;
	
	msg.what = JE_FRAMERATE_CHANGED;
	msg.AddInt64("framerate", 33000);
	msg.AddInt32("which", data->id);
	data->graphics_pipeline->SendMessage(&msg);
	
	msg.RemoveName("framerate");
	msg.what = JE_FRAME_SIZE_CHANGED;
	msg.AddRect("rect", data->outSize);
	data->graphics_pipeline->SendMessage(&msg);
	
	msg.RemoveName("rect");
	msg.what = JE_SOURCE_READY;
	data->graphics_pipeline->SendMessage(&msg);
	
	while (1)
	{
		data->nextFrame = system_time() + 33000;
		snooze_until(data->nextFrame, B_SYSTEM_TIMEBASE);
		
		if (data->exit)
			break;
		
		drawBitmap.RemoveName("bitmap");
		drawBitmap.AddPointer("bitmap", data->frame);
		data->window->SendMessage(&drawBitmap);
	}
	// it's ok to kill us now
	release_sem(data->exitSem);
}

void stub_source_exit(je_source_plugin *cookie)
{
	source_private *data = (source_private *)cookie->data;
	data->exit = true;
	// block
	acquire_sem(data->exitSem);
}

void stub_source_free(je_source_plugin *cookie)
{
	source_private *data = (source_private *)cookie->data;
	
	delete data->uiView;
	delete data->frame;
	delete data->graphics_pipeline;
	delete data->window;
	delete_sem(data->exitSem);
	
	delete data;
}

bool stub_source_lock(je_source_plugin *cookie)
{
	return true;
}

void stub_source_unlock(je_source_plugin *cookie)
{
}

BView *stub_source_getui(je_source_plugin *cookie)
{
	source_private *data = (source_private *)cookie->data;
	
	return data->uiView;
}

BBitmap *stub_source_getframe(je_source_plugin *cookie)
{
	source_private *data = (source_private *)cookie->data;
	
	return data->frame;
}

BRect stub_source_outputsize(je_source_plugin *cookie)
{
	source_private *data = (source_private *)cookie->data;
	
	return data->outSize;
}

bigtime_t stub_source_nextframetime(je_source_plugin *cookie)
{
	source_private *data = (source_private *)cookie->data;
	
	return data->nextFrame;
}
