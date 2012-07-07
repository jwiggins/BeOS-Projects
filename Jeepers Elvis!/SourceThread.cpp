/*
	SourceThread.cpp
	2000 John Wiggins
*/

#include "SourceThread.h"


SourceThread::SourceThread(const char *name, thread_id *thread)
{
	// spawn our work thread
	thid = spawn_thread(start_loop, name, B_DISPLAY_PRIORITY, (void *)this);
	if (thid > 0)
		resume_thread(thid);
	else
		thid = -1;
	
	// we don't have a plugin yet
	plugin = NULL;

	// or an output window
	outputWin = NULL;

	// return the thread id
	*thread = thid;
}

SourceThread::~SourceThread()
{
	if (thid > 0)
		kill_thread(thid);
		
	if (outputWin != NULL)
		delete outputWin;
}

void SourceThread::Start()
{
	if (plugin != NULL)
		plugin->start(plugin);
}

void SourceThread::Stop()
{
	if (plugin != NULL)
		plugin->stop(plugin);
}

void SourceThread::SetOutputWindow(BWindow *win)
{
	outputWin = new BMessenger(win);
	if (!outputWin->IsValid())
	{
		delete outputWin;
		outputWin = NULL;
	}
}

int32 SourceThread::start_loop(void *arg)
{
	return ((SourceThread *)arg)->main_loop();
}

int32 SourceThread::main_loop()
{
	thread_id sender;
	
	while (1)
	{
		// get a plugin
		receive_data(&sender, (void *)&plugin, sizeof(plugin));
		
		// run a plugin
		plugin->run(plugin);
		
		// tell the output window to show color bars
		if (outputWin != NULL)
		{
			BMessage draw(JE_DRAW_BITMAP);
			draw.AddPointer("bitmap", NULL);
			outputWin->SendMessage(&draw);
		}
	}
	return B_OK;
}

