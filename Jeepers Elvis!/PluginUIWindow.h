/*
	PluginUIWindow.h
	PluginUIWindow and PluginUIView class declarations for Jeepers Elvis!
	2000 John Wiggins
*/

#ifndef PLUGIN_UI_WINDOW_H
#define PLUGIN_UI_WINDOW_H

#include <interface/Window.h>
#include <interface/View.h>
#include <app/Message.h>
#include <app/Application.h>
#include <posix/stdio.h>

#include "MessageConstants.h"

class PluginUIWindow : public BWindow {
public:
						PluginUIWindow(BRect frame, const char *name);
						
	virtual	void		MessageReceived(BMessage *msg);
	virtual	bool		QuitRequested();

private:

	void				SwapView(BView *view);
//	class PluginUIView : public BView {
//	public:
//						PluginUIView(BRect frame, const char *name);
//						~PluginUIView();
//	
//		virtual	void	AttachedToWindow();
//		virtual	void	GetPreferredSize(float *width, float *height);
//		virtual	void	ResizeToPreferred();
//		void			SwapView(BView *view);
//	private:
//	
//		BView			*plugin;
//	};
	
//	PluginUIView		*pluginUI;
	BView				*pluginUI;
	bool				okToClose;
};

#endif