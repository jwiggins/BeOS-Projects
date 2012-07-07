/*
	MainWindow.cpp
	2000 John Wiggins
*/

#include "MainWindow.h"
#include "JEApp.h"

//////////////
// MainWindow
//////////////
MainWindow::MainWindow(BRect frame, const char *name)
: BWindow(frame, name, B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_ZOOMABLE|B_NOT_RESIZABLE)
{
	BRect rect;
	JEApp *appObj = (JEApp *)be_app; // for Resource Strings
	BMenuBar *menuBar;
	float menuBottom;
	
	// make the menu bar
	rect = Bounds();
	menuBar = new BMenuBar(rect, "menubar");
	
	// add menu(s) to menu bar
	// file menu
	menuBar->AddItem(new BMenu(appObj->ResString(STRING_FILE)));
	menuBar->SubmenuAt(0)->AddItem(new BMenuItem(appObj->ResString(STRING_ABOUT),
								new BMessage(B_ABOUT_REQUESTED), 'A', B_COMMAND_KEY));
	menuBar->SubmenuAt(0)->AddItem(new BMenuItem(appObj->ResString(STRING_QUIT),
								new BMessage(B_QUIT_REQUESTED), 'Q', B_COMMAND_KEY));
	menuBar->SubmenuAt(0)->ItemAt(0)->SetTarget(be_app);
	menuBar->SubmenuAt(0)->ItemAt(1)->SetTarget(be_app);
	// options menu
	menuBar->AddItem(new BMenu(appObj->ResString(STRING_OPTIONS)));
	menuBar->SubmenuAt(1)->AddItem(new BMenuItem(appObj->ResString(STRING_DISABLE_DEF_OUT),
								new BMessage(JE_UNSET_DEFAULT_OUTPUT)));
	menuBar->SubmenuAt(1)->ItemAt(0)->SetTarget(this);
	// add the menu bar
	AddChild(menuBar);
	// find out where the bottom of the menu bar is
	menuBottom = menuBar->Bounds().Height();
	//printf("menuBottom = %g\n", menuBottom);

	// add the main view
	mainView = new MainView(Bounds(), menuBottom);
	AddChild(mainView);
	
	// register with be_app
	Register();
	
	// construct all the plugin UI windows
	MakePluginUIWindows();
	
	// not ok to close yet
	okToClose = false;
	
	// show yourself
	Show();
}

void MainWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case JE_PLUGIN_LOADED:
		{
			const char *label;
			int32 type;
			int32 which;
			
			if (msg->FindString("desc", &label) == B_OK)
			{
				if (msg->FindInt32("type", &type) == B_OK)
				{
					if (msg->FindInt32("which", &which) == B_OK)
					{
						// tell mainView
						mainView->PluginLoaded(label, type, which);
					}
				}
			}
			
			break;
		}
		case JE_START_SOURCE:
		{
			int32 which;
			
			if (msg->FindInt32("which", &which) == B_OK)
			{
				if (which == 0)
				{
					// source 1
					mainView->src1Start->SetValue(0);
					mainView->src1Stop->SetValue(1);
				}
				else
				{
					// source 2
					mainView->src2Start->SetValue(0);
					mainView->src2Stop->SetValue(1);
				}
			}
			
			// foreward to be_app
			be_app->PostMessage(msg);
			
			break;
		}
		case JE_STOP_SOURCE:
		{
			int32 which;
			
			if (msg->FindInt32("which", &which) == B_OK)
			{
				if (which == 0)
				{
					// source 1
					mainView->src1Start->SetValue(1);
					mainView->src1Stop->SetValue(0);
				}
				else
				{
					// source 2
					mainView->src2Start->SetValue(1);
					mainView->src2Stop->SetValue(0);
				}
			}
			
			// foreward to be_app
			be_app->PostMessage(msg);
			
			break;
		}
		case JE_GET_UI:
		{
			int32 type;
			int32 which;
			void *view = NULL;
			BMessage reply;
			
			if (msg->FindInt32("type", &type) == B_OK)
			{
				if (msg->FindInt32("which", &which) == B_OK)
				{
					// handle filter special case
					if (type == JE_FILTER_PLUGIN)
					{
						which = mainView->filters->CurrentSelection();
						msg->RemoveName("which");
						msg->AddInt32("which", which);
					}
					// send to be_app
					BMessage uiMsg(*msg);
					be_app_messenger.SendMessage(&uiMsg, &reply);
					// harvest the view from the reply
					if (reply.FindPointer("view", &view) == B_OK && view != NULL)
					{
						//printf("MainWindow::MessageReceived(JE_GET_UI) : view = %p\n", view);
						// change the what
						reply.what = JE_LOAD_UI;
						// send it off to a PluginUIWindow
						switch(type)
						{
							case JE_SOURCE_PLUGIN:
							{
								if (which == 0)
									src1Win->PostMessage(&reply);
								else
									src2Win->PostMessage(&reply);
								break;
							}
							case JE_MIXER_PLUGIN:
							{
								mixWin->PostMessage(&reply);
								break;
							}
							case JE_FILTER_PLUGIN:
							{
								filtWin->PostMessage(&reply);
								break;
							}
							case JE_OUTPUT_PLUGIN:
							{
								outWin->PostMessage(&reply);
								break;
							}
							default:
								break;
						}
					}
				}
			}
			
			break;
		}
		case JE_UNSET_DEFAULT_OUTPUT:
		{
			// toggle check mark on "Disable Default Output" menu item
			BMenuBar *keyMenu = KeyMenuBar();
			
			if (keyMenu != NULL)
			{
				if (!keyMenu->SubmenuAt(1)->ItemAt(0)->IsMarked())
				{
					// mark
					keyMenu->SubmenuAt(1)->ItemAt(0)->SetMarked(true);
					// relay
					be_app->PostMessage(msg);
				}
				else
				{
					// unmark
					keyMenu->SubmenuAt(1)->ItemAt(0)->SetMarked(false);
					// modify
					msg->what = JE_SET_DEFAULT_OUTPUT;
					// relay
					be_app->PostMessage(msg);
				}
			}
						
			break;
		}
		case JE_APP_IS_QUITTING:
		{
			// it's ok to close now
			okToClose = true;
			// kill all the plugin windows
			src1Win->PostMessage(JE_APP_IS_QUITTING);
			src2Win->PostMessage(JE_APP_IS_QUITTING);
			mixWin->PostMessage(JE_APP_IS_QUITTING);
			filtWin->PostMessage(JE_APP_IS_QUITTING);
			outWin->PostMessage(JE_APP_IS_QUITTING);
			// tell yourself to quit
			PostMessage(B_QUIT_REQUESTED);
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}

bool MainWindow::QuitRequested()
{
	//printf("MainWindow::QuitRequested()\n");
	//printf("%s\n", Title());
	//Frame().PrintToStream();
	//Bounds().PrintToStream();
	
	// check to see if it's all right to quit
	if (!okToClose)
	{
		// tell be_app we wanna quit
		be_app->PostMessage(B_QUIT_REQUESTED);
		// don't close
		return false;
	}
	else
	{
		// un-register with be_app
		Unregister();
		// do close
		return true;
	}
}

void MainWindow::PluginDropped(BMessage *msg)
{
	switch(msg->what)
	{
		case JE_SOURCE_DND:
		{
			// One of the sources got a plugin
			// add the plugin type to the message
			msg->AddInt32("type", JE_SOURCE_PLUGIN);
			break;
		}
		case JE_MIXER_DND:
		{
			// The mixer got a plugin
			// add the plugin type to the message
			msg->AddInt32("type", JE_MIXER_PLUGIN);
			break;
		}
		case JE_FILTER_DND:
		{
			// The filter list got a plugin
			// add the plugin type to the message
			msg->AddInt32("type", JE_FILTER_PLUGIN);
			break;
		}
		case JE_OUTPUT_DND:
		{
			// The output got a plugin
			// add the plugin type to the message
			msg->AddInt32("type", JE_OUTPUT_PLUGIN);
			break;
		}
		default:
		{
			break;
		}
	}
	// change the what to something be_app knows
	msg->what = JE_LOAD_PLUGIN;
	// send it to be_app
	be_app->PostMessage(msg);
}

void MainWindow::UnloadPlugin(int32 type, int32 which)
{
	BMessage unload(JE_UNLOAD_UI);
	
	// add "type" and "which"
	unload.AddInt32("type", type);
	unload.AddInt32("which", which);
	unload.AddInt32("kill", 1);
	
	// if the plugin is a source plugin
	// disable it's start and stop buttons
	if (type == JE_SOURCE_PLUGIN)
	{
		if (which == 0)
		{
			mainView->src1Start->SetEnabled(false);
			mainView->src1Stop->SetEnabled(false);
		}
		else
		{
			mainView->src2Start->SetEnabled(false);
			mainView->src2Stop->SetEnabled(false);
		}
	}
	
	// unload the ui for this plugin
	UnloadPluginUI(type, which, unload);
	
	// send if off to be_app
	//be_app->PostMessage(&unload);
	// !! Now done in PluginUIWindow::MessageReceived() !!
}

void MainWindow::UnloadPluginUI(int32 type, int32 which, BMessage &msg)
{
	// unload the ui for a plugin
	switch(type)
	{
		case JE_SOURCE_PLUGIN:
		{
			if (which == 0)
				src1Win->PostMessage(&msg);
			else
				src2Win->PostMessage(&msg);
			break;
		}
		case JE_MIXER_PLUGIN:
		{
			mixWin->PostMessage(&msg);
			break;
		}
		case JE_FILTER_PLUGIN:
		{
			filtWin->PostMessage(&msg);
			break;
		}
		case JE_OUTPUT_PLUGIN:
		{
			outWin->PostMessage(&msg);
			break;
		}
		default:
			break;
	}
}

void MainWindow::MoveFilterPlugin(int32 from, int32 to)
{
	BMessage move(JE_MOVE_FILTER);
	
	// add "from" and "to"
	move.AddInt32("from", from);
	move.AddInt32("to", to);
	
	// send if off to be_app
	be_app->PostMessage(&move);
}

void MainWindow::MakePluginUIWindows()
{
	JEApp *appObj = (JEApp *)be_app; // for Resource Strings
	
	// Source1 plugin window
	src1Win = new PluginUIWindow(BRect(100.0,100.0,101.0,101.0), appObj->ResString(STRING_SOURCE1_UI));
	//src1Win->AddToSubset(this);
	// Source2 plugin window
	src2Win = new PluginUIWindow(BRect(110.0,110.0,111.0,111.0), appObj->ResString(STRING_SOURCE2_UI));
	//src2Win->AddToSubset(this);
	// Mixer plugin window
	mixWin = new PluginUIWindow(BRect(120.0,120.0,121.0,121.0), appObj->ResString(STRING_MIXER_UI));
	//mixWin->AddToSubset(this);
	// Filter plugin window
	filtWin = new PluginUIWindow(BRect(130.0,130.0,131.0,131.0), appObj->ResString(STRING_FILTER_UI));
	//filtWin->AddToSubset(this);
	// Output plugin window
	outWin = new PluginUIWindow(BRect(140.0,140.0,141.0,141.0), appObj->ResString(STRING_OUTPUT_UI));
	//outWin->AddToSubset(this);
}

void MainWindow::Register()
{
	BMessage registry(JE_WINDOW_REGISTRY_ADD);
	be_app->PostMessage(&registry);
}

void MainWindow::Unregister()
{
	BMessage registry(JE_WINDOW_REGISTRY_SUB);
	be_app->PostMessage(&registry);
}


////////////
// MainView
////////////
MainWindow::MainView::MainView(BRect frame, float menuBottom)
: BView(frame, "MainView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	BRect iconRect(0.0,0.0,31.0,31.0), rect, bounds = Bounds();
	BMessage *getUIMsg;
	BButton *uiButton;
	BBox *containerBox;
	JEApp *appObj = (JEApp *)be_app; // for Resource Strings
	BResources *resources = be_app->AppResources();
	BBitmap *resIcon;
	const void *iconBits;
	size_t bitsSize;
	
	// set the view color
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	// get the source icon
	iconBits = resources->LoadResource('ICON', 1, &bitsSize);
	
	////////////
	// Source 1
	////////////
	// DropView, BButton and BBox for Source 1
	// first, the BBox to surround it all
	rect.Set(0.0, 0.0, 80.0, 60.0);
	rect.OffsetTo(bounds.Width()/8.0, menuBottom+5.0);
	containerBox = new BBox(rect, "source1box");
	containerBox->SetLabel(appObj->ResString(STRING_SOURCE1));
	AddChild(containerBox);
	// then the DropView
	// make the icon
	resIcon = new BBitmap(iconRect, B_CMAP8);
	resIcon->SetBits(iconBits, bitsSize, 0, B_CMAP8);
	// set up the bounds
	source1Rect = resIcon->Bounds();
	source1Rect.InsetBy(-3.0, -3.0); // room for border and highlight
	source1Rect.OffsetTo(5.0, 20.0);
	// construct and add the view
	containerBox->AddChild(
		source1 = new DropView(source1Rect, "Source1", resIcon, JE_SOURCE_DND, JE_SOURCE_PLUGIN, 0));
	// forget about resIcon
	resIcon = NULL;
	// finally, the UI button
	rect.Set(0.0, 0.0, 25.0, 16.0);
	rect.OffsetTo(source1Rect.right + 5.0, source1Rect.top + 10.0);
	getUIMsg = new BMessage(JE_GET_UI);
	getUIMsg->AddInt32("type", JE_SOURCE_PLUGIN); // source
	getUIMsg->AddInt32("which", 0); // 1
	containerBox->AddChild(uiButton = new BButton(rect, "source1ui", appObj->ResString(STRING_UI), getUIMsg));
	getUIMsg = NULL; // the button keeps it
	// tell the DropView about its button
	source1->SetUIButton(uiButton);
	// expand the rect
	source1Rect = containerBox->Frame(); // the rect of the container BBox
	source1Rect.InsetBy(-2.0,-2.0);
	// forget the container and the button
	uiButton = NULL;
	containerBox = NULL;
	
	////////////
	// Source 2
	////////////
	// DropView, BButton and BBox for Source 2
	// first, the BBox to surround it all
	rect.Set(0.0, 0.0, 80.0, 60.0);
	rect.OffsetTo(bounds.right-(3.0*(bounds.Width()/8.0)), menuBottom+5.0);
	containerBox = new BBox(rect, "source2box");
	containerBox->SetLabel(appObj->ResString(STRING_SOURCE2));
	AddChild(containerBox);
	// the the DropView
	// make the icon
	resIcon = new BBitmap(iconRect, B_CMAP8);
	resIcon->SetBits(iconBits, bitsSize, 0, B_CMAP8);
	// set up the bounds
	source2Rect = resIcon->Bounds();
	source2Rect.InsetBy(-3.0, -3.0); // room for border and highlight
	source2Rect.OffsetTo(5.0, 20.0);
	// construct and add the view
	containerBox->AddChild(
		source2 = new DropView(source2Rect, "Source2", resIcon, JE_SOURCE_DND, JE_SOURCE_PLUGIN, 1));
	// forget about resIcon
	resIcon = NULL;
	// finally, the UI button
	rect.Set(0.0, 0.0, 25.0, 16.0);
	rect.OffsetTo(source2Rect.right + 5.0, source2Rect.top + 10.0);
	getUIMsg = new BMessage(JE_GET_UI);
	getUIMsg->AddInt32("type", JE_SOURCE_PLUGIN); // source
	getUIMsg->AddInt32("which", 1); // 2
	containerBox->AddChild(uiButton = new BButton(rect, "source2ui", appObj->ResString(STRING_UI), getUIMsg));
	getUIMsg = NULL; // the button keeps it
	// tell the DropView about its button
	source2->SetUIButton(uiButton);
	// expand the rect
	source2Rect = containerBox->Frame(); // the rect of the container BBox
	source2Rect.InsetBy(-2.0,-2.0);
	// forget the container and the button
	uiButton = NULL;
	containerBox = NULL;
	
	// get the mixer icon
	iconBits = resources->LoadResource('ICON', 2, &bitsSize);
	
	/////////
	// Mixer
	/////////
	// DropView, BButton and BBox for the Mixer
	// first, the BBox to surround it all
	rect.Set(0.0, 0.0, 80.0, 60.0);
	rect.OffsetTo((bounds.Width()/2.0)-(rect.Width()/2.0), source2Rect.bottom+10.0);
	containerBox = new BBox(rect, "mixerbox");
	containerBox->SetLabel(appObj->ResString(STRING_MIXER));
	AddChild(containerBox);
	// then the DropView
	// make the icon
	resIcon = new BBitmap(iconRect, B_CMAP8);
	resIcon->SetBits(iconBits, bitsSize, 0, B_CMAP8);
	// set up the bounds
	mixerRect = resIcon->Bounds();
	mixerRect.InsetBy(-3.0, -3.0); // room for border and highlight
	mixerRect.OffsetTo(5.0, 20.0);
	// construct and add the view
	containerBox->AddChild(
		mixer = new DropView(mixerRect, "Mixer", resIcon, JE_MIXER_DND, JE_MIXER_PLUGIN, 0));
	// forget about resIcon
	resIcon = NULL;
	// finally, the UI button
	rect.Set(0.0, 0.0, 25.0, 16.0);
	rect.OffsetTo(mixerRect.right + 5.0, mixerRect.top + 10.0);
	getUIMsg = new BMessage(JE_GET_UI);
	getUIMsg->AddInt32("type", JE_MIXER_PLUGIN); // mixer
	getUIMsg->AddInt32("which", 0);
	containerBox->AddChild(uiButton = new BButton(rect, "mixerui", appObj->ResString(STRING_UI), getUIMsg));
	getUIMsg = NULL; // the button keeps it
	// tell the DropView about its button
	mixer->SetUIButton(uiButton);
	// expand the rect
	mixerRect = containerBox->Frame(); // the rect of the container BBox
	mixerRect.InsetBy(-2.0,-2.0);
	// forget the container and the button
	uiButton = NULL;
	containerBox = NULL;
	
	// get the output icon
	iconBits = resources->LoadResource('ICON', 4, &bitsSize);
	
	//////////
	// Output
	//////////
	// DropView, BButton and BBox for the Output
	// first, the BBox to surround it all
	rect.Set(0.0, 0.0, 80.0, 60.0);
	rect.OffsetTo((bounds.Width()/2.0)-(rect.Width()/2.0), bounds.bottom-rect.Height()-5.0);
	containerBox = new BBox(rect, "mixerbox");
	containerBox->SetLabel(appObj->ResString(STRING_OUTPUT));
	AddChild(containerBox);
	// then the DropView
	// make the icon
	resIcon = new BBitmap(iconRect, B_CMAP8);
	resIcon->SetBits(iconBits, bitsSize, 0, B_CMAP8);
	// set up the bounds
	outputRect = resIcon->Bounds();
	outputRect.InsetBy(-3.0, -3.0); // room for border and highlight
	outputRect.OffsetTo(5.0, 20.0);
	// construct and add the view
	containerBox->AddChild(
		output = new DropView(outputRect, "Output", resIcon, JE_OUTPUT_DND, JE_OUTPUT_PLUGIN, 0));
	// forget about resIcon
	resIcon = NULL;
	// finally, the UI button
	rect.Set(0.0, 0.0, 25.0, 16.0);
	rect.OffsetTo(outputRect.right + 5.0, outputRect.top + 10.0);
	getUIMsg = new BMessage(JE_GET_UI);
	getUIMsg->AddInt32("type", JE_OUTPUT_PLUGIN); // output
	getUIMsg->AddInt32("which", 0);
	containerBox->AddChild(uiButton = new BButton(rect, "outputui", appObj->ResString(STRING_UI), getUIMsg));
	getUIMsg = NULL; // the button keeps it
	// tell the DropView about its button
	output->SetUIButton(uiButton);
	// expand the rect
	outputRect = containerBox->Frame(); // the rect of the container BBox
	outputRect.InsetBy(-2.0,-2.0);
	// forget the container and the button
	uiButton = NULL;
	containerBox = NULL;
	
	///////////
	// Filters
	///////////
	// FilterListView, BButton and BBox for the Filters
	// first, the BBox to surround it all
	rect.Set(0.0, 0.0, bounds.Width()-30.0, outputRect.top-mixerRect.bottom-40.0);
	rect.OffsetTo(15.0, mixerRect.bottom+20.0);
	containerBox = new BBox(rect, "filterbox");
	containerBox->SetLabel(appObj->ResString(STRING_FILTERS));
	AddChild(containerBox);
	// then the FilterListView
	filtersRect.Set(0.0, 0.0, rect.Width()-50.0-B_V_SCROLL_BAR_WIDTH, rect.Height()-30.0);
	filtersRect.OffsetTo(7.0, 20.0);
	filters = new FilterListView(filtersRect,"Filters", JE_FILTER_DND);
	containerBox->AddChild(
		new BScrollView("filterscroll",filters,B_FOLLOW_LEFT|B_FOLLOW_TOP,0,false,true));
	// finally, the UI button
	rect.Set(0.0, 0.0, 25.0, 16.0);
	rect.OffsetTo(filtersRect.right+B_V_SCROLL_BAR_WIDTH+5.0, filtersRect.bottom - 20.0);
	getUIMsg = new BMessage(JE_GET_UI);
	getUIMsg->AddInt32("type", JE_FILTER_PLUGIN); // filter
	getUIMsg->AddInt32("which", 0); // ignored
	containerBox->AddChild(uiButton = new BButton(rect, "outputui", appObj->ResString(STRING_UI), getUIMsg));
	getUIMsg = NULL; // the button keeps it
	// tell the FilterListView about its button
	filters->SetUIButton(uiButton);
	// expand the rect
	filtersRect = containerBox->Frame(); // the rect of the container BBox
	filtersRect.InsetBy(-2.0,-2.0);
	// forget the container and the button
	uiButton = NULL;
	containerBox = NULL;
}

//MainWindow::MainView::~MainView()
//{
//}

void MainWindow::MainView::AttachedToWindow()
{
	BRect bitRect(0.0,0.0,9.0,9.0), rect;
	BMessage *startStop;
	BBitmap *startMap, *stopMap;
	BPicture *startOn, *startOff, *stopOn, *stopOff;
	BView *containerBox;
	
	// make the start and stop button bitmaps
	// start
	startMap = new BBitmap(bitRect, B_RGB32);
	startMap->SetBits(kStartImageBits, sizeof(kStartImageBits), 0, B_RGB32);
	// stop
	stopMap = new BBitmap(bitRect, B_RGB32);
	stopMap->SetBits(kStopImageBits, sizeof(kStopImageBits), 0, B_RGB32);
	// and the BPictures that use them
	// start button, on position
	BeginPicture(new BPicture);
	DrawBitmap(startMap,BPoint(0.0,0.0));
	startOn = EndPicture();
	// start button, off position
	BeginPicture(new BPicture);
	DrawBitmap(startMap,BPoint(0.0,0.0));
	SetDrawingMode(B_OP_BLEND);
	SetHighColor(192,192,192,255);
	FillRect(startMap->Bounds());
	startOff = EndPicture();
	// stop button, on position
	SetDrawingMode(B_OP_COPY);
	SetHighColor(0,0,0,255);
	BeginPicture(new BPicture);
	DrawBitmap(stopMap,BPoint(0.0,0.0));
	stopOn = EndPicture();
	// stop button, off position
	BeginPicture(new BPicture);
	DrawBitmap(stopMap,BPoint(0.0,0.0));
	SetDrawingMode(B_OP_BLEND);
	SetHighColor(192,192,192,255);
	FillRect(stopMap->Bounds());
	stopOff = EndPicture();
	
	// the start and stop buttons for source 1
	// the container for the buttons
	containerBox = FindView("source1box");
	// start button
	rect = bitRect;
	rect.OffsetTo(45.0, 15.0);
	startStop = new BMessage(JE_START_SOURCE);
	startStop->AddInt32("which", 0); // source 1
	containerBox->AddChild(
		src1Start = new BPictureButton(rect, "start1",new BPicture(*startOff),new BPicture(*startOn),startStop,B_TWO_STATE_BUTTON));
	// stop button
	rect.OffsetTo(rect.right+1.0,rect.top);
	startStop = new BMessage(JE_STOP_SOURCE);
	startStop->AddInt32("which", 0); // source 1
	containerBox->AddChild(
		src1Stop = new BPictureButton(rect, "stop1",new BPicture(*stopOff),new BPicture(*stopOn),startStop,B_TWO_STATE_BUTTON));
	
	// the start and stop buttons for source 2
	// the container for the buttons
	containerBox = FindView("source2box");
	// start button
	rect = bitRect;
	rect.OffsetTo(45.0, 15.0);
	startStop = new BMessage(JE_START_SOURCE);
	startStop->AddInt32("which", 1); // source 2
	containerBox->AddChild(
		src2Start = new BPictureButton(rect, "start2",new BPicture(*startOff),new BPicture(*startOn),startStop,B_TWO_STATE_BUTTON));
	// stop button
	rect.OffsetTo(rect.right+1.0,rect.top);
	startStop = new BMessage(JE_STOP_SOURCE);
	startStop->AddInt32("which", 1); // source 2
	containerBox->AddChild(
		src2Stop = new BPictureButton(rect, "stop2",new BPicture(*stopOff),new BPicture(*stopOn),startStop,B_TWO_STATE_BUTTON));
	
	// assign the disabled state pictures
	src1Start->SetDisabledOn(new BPicture(*startOff));
	src1Stop->SetDisabledOn(new BPicture(*stopOff));
	src2Start->SetDisabledOn(new BPicture(*startOff));
	src2Stop->SetDisabledOn(new BPicture(*stopOff));
	src1Start->SetDisabledOff(new BPicture(*startOff));
	src1Stop->SetDisabledOff(new BPicture(*stopOff));
	src2Start->SetDisabledOff(new BPicture(*startOff));
	src2Stop->SetDisabledOff(new BPicture(*stopOff));
	
	// set up the starting button states
	src1Start->SetValue(0);
	src1Stop->SetValue(1);
	src2Start->SetValue(0);
	src2Stop->SetValue(1);
	// disable start and stop buttons
	src1Start->SetEnabled(false);
	src1Stop->SetEnabled(false);
	src2Start->SetEnabled(false);
	src2Stop->SetEnabled(false);
	
	// restore drawing state
	SetDrawingMode(B_OP_COPY);
	SetHighColor(0,0,0,255);
	
	// clean up the start and stop pictures
	delete startOn;
	delete startOff;
	delete stopOn;
	delete stopOff;
	// and bitmaps
	delete startMap;
	delete stopMap;
}

void MainWindow::MainView::Draw(BRect update)
{
	float center = Bounds().Width()/2.0f;
	
	// set the pen size
	SetPenSize(2.0);
	// outline the plugin container views
//	StrokeRect(source1Rect);
//	StrokeRect(source2Rect);
//	StrokeRect(mixerRect);
//	StrokeRect(filtersRect);
//	StrokeRect(outputRect);
	// connect the dots
	StrokeLine(BPoint(source1Rect.left+(source1Rect.Width()/2.0),source1Rect.bottom),
		BPoint(source1Rect.left+(source1Rect.Width()/2.0),mixerRect.top+(mixerRect.Height()/2.0)));
	StrokeLine(BPoint(source1Rect.left+(source1Rect.Width()/2.0),mixerRect.top+(mixerRect.Height()/2.0)),
		BPoint(mixerRect.left, mixerRect.top+(mixerRect.Height()/2.0)));
	StrokeLine(BPoint(source2Rect.left+(source2Rect.Width()/2.0),source2Rect.bottom),
		BPoint(source2Rect.left+(source2Rect.Width()/2.0),mixerRect.top+(mixerRect.Height()/2.0)));
	StrokeLine(BPoint(source2Rect.left+(source2Rect.Width()/2.0),mixerRect.top+(mixerRect.Height()/2.0)),
		BPoint(mixerRect.right, mixerRect.top+(mixerRect.Height()/2.0)));
	StrokeLine(BPoint(center, mixerRect.bottom), BPoint(center, filtersRect.top));
	StrokeLine(BPoint(center, filtersRect.bottom), BPoint(center, outputRect.top));
}

void MainWindow::MainView::PluginLoaded(const char *label, int32 type, int32 which)
{
	switch(type)
	{
		case JE_SOURCE_PLUGIN:
		{
			if (which == 0)
			{
				// toggle occupancy
				source1->ToggleOccupant(true);
				// enable start and stop buttons
				src1Start->SetEnabled(true);
				src1Stop->SetEnabled(true);
				// ensure state
				src1Start->SetValue(0);
				src1Stop->SetValue(1);
			}
			else
			{
				source2->ToggleOccupant(true);
				// enable start and stop buttons
				src2Start->SetEnabled(true);
				src2Stop->SetEnabled(true);
				// ensure state
				src2Start->SetValue(0);
				src2Stop->SetValue(1);
			}
			break;
		}
		case JE_MIXER_PLUGIN:
		{
			mixer->ToggleOccupant(true);
			break;
		}
		case JE_FILTER_PLUGIN:
		{
			filters->AddFilterItem(label);
			break;
		}
		case JE_OUTPUT_PLUGIN:
		{
			output->ToggleOccupant(true);
			break;
		}
		default:
		{
			break;
		}
	}
}
