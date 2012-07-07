/*
	PluginUIWindow.cpp
	2000 John Wiggins
*/

#include "PluginUIWindow.h"

//////////////////
// PluginUIWindow
//////////////////
PluginUIWindow::PluginUIWindow(BRect frame, const char *name)
: BWindow(frame, name, B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_ZOOMABLE/*|B_ASYNCHRONOUS_CONTROLS*/)
{
	// construct and add the child PluginUIView
	//AddChild(pluginUI = new PluginUIView(Bounds(), "pluginUI"));
	pluginUI = NULL;
	
	// register with be_app
	BMessage registry(JE_WINDOW_REGISTRY_ADD);
	be_app->PostMessage(&registry);
	
	// better not close (even if told)
	okToClose = false;
	
	// start the message loop
	Hide();
	Show();
}

void PluginUIWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case JE_LOAD_UI:
		{
			void *view = NULL;
			
			if (msg->FindPointer("view", &view) == B_OK)
			{
				//printf("PluginUIWindow::MessageReceived(JE_LOAD_UI)\n");
				//printf("view = %p\n", view);
				// show the window if we're hidden
				if (IsHidden())
					Show();
				// swap out the view
				//pluginUI->SwapView(reinterpret_cast<BView *>(view));
				SwapView(reinterpret_cast<BView *>(view));
			}
			
			break;
		}
		case JE_UNLOAD_UI:
		{
			//printf("PluginUIWindow::MessageReceived(JE_UNLOAD_UI)\n");
			int32 kill=0;
			
			// just send it a NULL pointer
			//pluginUI->SwapView(NULL);
			SwapView(NULL);
			
			// hide the window (unless it's already hidden)
			if (!IsHidden())
				Hide();
			
			// we only unload the plugin if "kill" is in the msg
			// otherwise we just unload the UI here
			if (msg->FindInt32("kill", &kill) == B_OK)
			{
				BMessage app_msg(*msg); // copy
				// change the 'what' of app_msg
				app_msg.what = JE_UNLOAD_PLUGIN;
				// send
				be_app->PostMessage(&app_msg);
			}
			
			break;
		}
		case JE_APP_IS_QUITTING:
		{
			// we're quitting
			// it's now ok to kill ourself
			okToClose = true;
			PostMessage(B_QUIT_REQUESTED);
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}

bool PluginUIWindow::QuitRequested()
{
	//printf("%s\n", Title());
	//Frame().PrintToStream();
	//Bounds().PrintToStream();
	
	// don't close if its not time yet
	if (!okToClose)
	{
		// hide instead
		if (!IsHidden())
			Hide();
		return false;
	}
	else
	{
		// quittin' time
		// unregister with be_app
		BMessage registry(JE_WINDOW_REGISTRY_SUB);
		be_app->PostMessage(&registry);
		return true;
	}
}

void PluginUIWindow::SwapView(BView *view)
{
	if (view == NULL)
	{
		// unload current view
		if (pluginUI != NULL)
		{
			// remove
			//RemoveChild(pluginUI);
			pluginUI->RemoveSelf();
			// forget
			pluginUI = NULL;
			// revert to minimal size
			ResizeTo(5.0,5.0);
			// resize
			//ResizeToPreferred();
		}
	}
	else if (view != pluginUI)
	{
		// new view...
			
		// cache the new pointer
		pluginUI = view;
		// make sure its in the right place
		pluginUI->MoveTo(0.0,0.0);
		// install
		AddChild(pluginUI);
		// resize
		pluginUI->ResizeToPreferred();
		// resize window
		ResizeTo(pluginUI->Bounds().Width(), pluginUI->Bounds().Height());
	}
}

//////////////
// PluginUIView
//////////////
//PluginUIWindow::PluginUIView::PluginUIView(BRect frame, const char *name)
//: BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW)
//{
//	// no plugin loaded yet
//	plugin = NULL;
//	
//	// set our view color
//	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
//}
//
//PluginUIWindow::PluginUIView::~PluginUIView()
//{
//}
//
//void PluginUIWindow::PluginUIView::AttachedToWindow()
//{
//	ResizeToPreferred();
//}
//
//void PluginUIWindow::PluginUIView::GetPreferredSize(float *width, float *height)
//{
//	if (plugin != NULL)
//		plugin->GetPreferredSize(width, height);
//	else
//	{
//		// yeah, these look good...
//		*width = Frame().Width();
//		*height = Frame().Height();
//	}
//}
//
//void PluginUIWindow::PluginUIView::ResizeToPreferred()
//{
//	float w,h;
//	// get the preferred size
//	GetPreferredSize(&w,&h);
//	// resize the view
//	ResizeTo(w,h);
//	// resize the window to fit
//	Window()->ResizeTo(w, h);
//}
//
//void PluginUIWindow::PluginUIView::SwapView(BView *view)
//{
//	//printf("PluginUIView::SwapView()\n");
//	//printf("plugin = %p\n", plugin);
//	//printf("view = %p\n", view);
//	if (view == NULL)
//	{
//		// unload current view
//		if (plugin != NULL)
//		{
//			// remove
//			//printf("RemoveChild(plugin) = %d\n", RemoveChild(plugin));
//			RemoveChild(plugin);
//			// forget
//			plugin = NULL;
//			// resize
//			ResizeToPreferred();
//		}
//	}
//	else if (view != plugin)
//	{
//		// new view...
//			
//		// cache the new pointer
//		plugin = view;
//		// make sure its in the right place
//		plugin->MoveTo(0.0,0.0);
//		// install
//		AddChild(plugin);
//		// resize
//		ResizeToPreferred();
//	}
//	else
//		; // we already have this view... nothing changes
//}

