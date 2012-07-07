/*
	SourceThread.h
	Class for managing sources and the threads they live in
	2000 John Wiggins
*/

#ifndef SOURCE_THREAD_H
#define SOURCE_THREAD_H

#include <app/Message.h>
#include <app/Messenger.h>
#include <interface/Window.h>
#include <kernel/OS.h>

#include "JEPlugin.h"
#include "MessageConstants.h"

class SourceThread {
public:
						SourceThread(const char *name, thread_id *thread);
						~SourceThread();
	
	void				Start();
	void				Stop();
	
	void				SetOutputWindow(BWindow *win);
	
private:
	static	int32		start_loop(void *arg);
	int32				main_loop();
	
	thread_id			thid;
	je_source_plugin	*plugin;
	BMessenger			*outputWin;
};

#endif