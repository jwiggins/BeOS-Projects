/*
	
	Sketch.cpp
	
	John Wiggins 1998
	
*/


#include "SketchWindow.h"
#include "SketchView.h"
#include "Sketch.h"


int main()
{	
	SketchApp *myApplication;

	myApplication = new SketchApp();
	myApplication->Run();
	
	delete(myApplication);
	return(0);
}

SketchApp::SketchApp()
		  		  : BApplication("application/x-vnd.Prok-Sketch")
{
	// most of this function was ripped out of Be's old HelloWorld Example
	SketchWindow	*mainWindow = NULL;
	SketchView		*aView = NULL;
	BMenuBar		*myMenubar = NULL;
	BMenu			*firstMenu = NULL, *secondMenu = NULL;
	BRect			aRect;
	menu_info		m_info;
	int32			menubarheight=0;
	
	// the price we pay to be font sensitive...
	get_menu_info(&m_info);
	menubarheight = (int32)m_info.font_size + 8;
	aRect.Set(100, 100, 449, 349 + menubarheight);
	mainWindow = new SketchWindow(aRect);
	
	// a pretty standard set of menus
	aRect.Set(0,0, mainWindow->Bounds().Width(), menubarheight);
	myMenubar = new BMenuBar(aRect, "menubar");
	// main menu
	firstMenu = new BMenu("Sketch");
	firstMenu->AddItem(new BMenuItem("About Sketch", new BMessage(B_ABOUT_REQUESTED), 'A', B_COMMAND_KEY));
	firstMenu->AddItem(new BMenuItem("Save", new BMessage(SAVE_BITMAP), 'S', B_COMMAND_KEY));
	firstMenu->AddItem(new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q', B_COMMAND_KEY));
	((BMenuItem *)firstMenu->ItemAt(0))->SetTarget(be_app);
	((BMenuItem *)firstMenu->ItemAt(1))->SetTarget(be_app);
	
#if __INTEL__ // only cuz the translation kit is busted in R3, will undo in R3.1
	//((BMenuItem *)firstMenu->ItemAt(1))->SetEnabled(false);
#endif
	// edit menu
	secondMenu = new BMenu("Edit");
	secondMenu->AddItem(new BMenuItem("Erase", new BMessage(ANIMATED_ERASE), 'E', B_COMMAND_KEY));
	myMenubar->AddItem(firstMenu);
	myMenubar->AddItem(secondMenu);
	
	// SketchView
	aRect.Set(0,menubarheight -1, mainWindow->Bounds().Width(), mainWindow->Bounds().Height());
	aView = new SketchView(aRect, "SketchView");
	
	// add views to window
	mainWindow->AddChild(aView);
	mainWindow->AddChild(myMenubar);
	
	// make window visible
	mainWindow->Show();
}

void SketchApp::AboutRequested()
{
	// standard BAlert foo
	BAlert *alert;
	alert = new BAlert("about", "Sketch v1.6\nBy John Wiggins 1998.\n", "neat");
	alert->Go();
}

void SketchApp::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case SAVE_BITMAP:
		{
			// show the file panel, when the user dismisses it the MessageRecieved func
			// in be_app will catch it and send it to the window
			saver->Show();
			break;
		}
		case B_SAVE_REQUESTED:
		{
			// redirect msg to the window. It knoweth what to do.
			((SketchWindow *)FindWindow("Etch-A-Sketch"B_UTF8_REGISTERED))->PostMessage(msg);
			break;
		}
		
		default:
			BApplication::MessageReceived(msg);
			break;
	}
}

void SketchApp::ReadyToRun()
{
	// Build a menu into your filepanel for fun and profit!
	SketchWindow	*mainWindow = ((SketchWindow *)FindWindow("Etch-A-Sketch"B_UTF8_REGISTERED));
	//BFilePanel		*saver = NULL;
	BMenuField		*formatMenu = NULL;
	BMenu			*menu = NULL;
	BView			*saverView = NULL;
	BRect			rect, bounds;
	menu_info		m_info;
	int32			menubarheight=0;
	
	// get some menu measurements
	get_menu_info(&m_info);
	menubarheight = (int32)m_info.font_size + 8;
	
	// make the filepanel
	saver = new BFilePanel(B_SAVE_PANEL, NULL, NULL, B_FILE_NODE, false); // _ctor
	
	if(saver->Window()->Lock())
	{
		// get a pointer to a view
		saverView = (BView *)saver->Window()->ChildAt(0);
	
		// get a rect for the filepanel's window
		bounds = saverView->Bounds();
	
		// make a menu
		menu = new BMenu("Format");
	
		// construct the BMenuFeild for the menu
		rect.Set(bounds.right - 100, bounds.top + menubarheight, bounds.right - 40, bounds.top + menubarheight + 15);
		formatMenu = new BMenuField(rect, "format menu", "", menu);
	
		formatMenu->SetDivider(0);
		
		// Below was ganked from the BeBook
		//find all the translators
		BTranslatorRoster *roster = BTranslatorRoster::Default(); 
		int32 num_translators, i; 
		translator_id *translators; 
		const char *translator_name, *translator_info;
		int32 translator_version; 

		roster->GetAllTranslators(&translators, &num_translators); // function allocates the memory for translators
		for (i=0;i<num_translators;i++)
		{ 
			const translation_format *fmts; 
        	int32 num_fmts;
			roster->GetTranslatorInfo(translators[i], &translator_name, &translator_info, &translator_version); 
   			roster->GetOutputFormats(translators[i], &fmts, &num_fmts);
   			
   			if(fmts[0].group == B_TRANSLATOR_BITMAP) // make sure we only get bitmap translators
   			{
   				BMessage *message = new BMessage(TRANSLATOR);
   				message->AddString("save format", fmts[0].MIME);
   				if(i == 0)
   					mainWindow->PostMessage(message); // this will init 'format' for us
					menu->AddItem(new BMenuItem(translator_name, message));
			}
			//printf("%s: %s (%.2f)\n", translator_name, translator_info, translator_version/100.); 
		} 
   
		delete [] translators; // clean up our droppings
		// found em all
		
		menu->SetRadioMode(true);
		menu->ItemAt(0)->SetMarked(true);
		menu->SetTargetForItems(mainWindow);
	
	
		saverView->AddChild(formatMenu);
		saver->Window()->Unlock();
	}
}

BWindow * SketchApp::FindWindow(const char *title)
{
	BWindow *win = NULL;
	int32 i = 0;
	while((win = be_app->WindowAt(i++)) != NULL)
	{
		if(win->Lock())
		{
			if((win->Title() != NULL) && !strcmp(win->Title(),title)/* strcmp is screwy */)
			{
				win->Unlock();
				return win;
			}
			win->Unlock();
		}
	}
	return win;
}

BFilePanel * SketchApp::FindFilePanel()
{
	BFilePanel *win = NULL;
	int32 i = 0;
	while((win = (BFilePanel *)be_app->WindowAt(i++)) != NULL)
	{
		if(!strcmp(class_name(win), "BFilePanel"))
			return cast_as(win, BFilePanel);
	}
	return NULL;
}