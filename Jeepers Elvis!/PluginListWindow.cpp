/*
	PluginListWindow.cpp
	2000 John Wiggins
*/

#include "PluginListWindow.h"

PluginListWindow::PluginListWindow(BRect frame, const char *name)
: BWindow(frame, name, B_DOCUMENT_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_CLOSABLE|B_NOT_ZOOMABLE|B_WILL_ACCEPT_FIRST_CLICK)
{
	// register with be_app
	BMessage registry(JE_WINDOW_REGISTRY_ADD);
	be_app->PostMessage(&registry);
	
	// construct your plugin list view
	BRect bounds = Bounds();
	PluginOutlineListView *listView;
	
	bounds.right -= B_V_SCROLL_BAR_WIDTH;
	bounds.bottom -= B_H_SCROLL_BAR_HEIGHT;
	listView = new PluginOutlineListView(bounds, "pluginlist");
	AddChild(new BScrollView("listscroll",listView,B_FOLLOW_ALL,0,true,true));
	
	// show yourself
	Show();
	
	// request plugins (reply goes to listView)
	BMessage plugins(JE_GET_AVAILABLE_PLUGINS);
	be_app->PostMessage(&plugins, NULL, listView);
}

void PluginListWindow::MessageReceived(BMessage *msg)
{
	//printf("PluginListWindow::MessageReceived()\n");
	switch(msg->what)
	{
		case JE_APP_IS_QUITTING:
		{
			// tell yourself to quit
			PostMessage(B_QUIT_REQUESTED);
			
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}

bool PluginListWindow::QuitRequested()
{
	//printf("%s\n", Title());
	//Frame().PrintToStream();
	//Bounds().PrintToStream();
	
	// unregister with be_app
	BMessage registry(JE_WINDOW_REGISTRY_SUB);
	be_app->PostMessage(&registry);
	return true;
}