/*
	PluginListWindow.h
	PluginListWindow class declaration for Jeepers Elvis!
	2000 John Wiggins
*/

#ifndef PLUGIN_LIST_WINDOW_H
#define PLUGIN_LIST_WINDOW_H

#include <interface/Window.h>
#include <interface/Rect.h>
#include <interface/ScrollView.h>
#include <app/Message.h>
#include <app/Application.h>
#include <posix/stdio.h>

#include "MessageConstants.h"
#include "PluginOutlineListView.h"

class PluginListWindow : public BWindow {
public:
							PluginListWindow(BRect frame, const char *name);
	
	virtual	void			MessageReceived(BMessage *msg);
	virtual	bool			QuitRequested();
private:
};

#endif