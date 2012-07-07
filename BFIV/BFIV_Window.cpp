#include "BFIV_Window.h"

BFIV_Window::BFIV_Window(BRect frame, const char *name)
: BWindow(frame, name, B_DOCUMENT_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_ZOOMABLE|B_NO_WORKSPACE_ACTIVATION), fFile_ref()
{
	BRect rect = Bounds();
	BMenuBar *menubar;
	
	// Hello be_app!
	be_app->PostMessage(WINDOW_REGISTRY_ADD);
	
	// build the menu(s) and get a pointer to the parent menubar
	menubar = BuildMenus();
	// add the menubar to the window
	AddChild(menubar);
	// get it's height
	rect.top = menubar->Bounds().bottom + 1;
	rect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	rect.right -= B_V_SCROLL_BAR_WIDTH;
	
	// construct the status view
	fStatus = new BFIV_Status(rect, "status");
	AddChild(fStatus);
	
	// add the scroll bars
	rect.left = Bounds().right - B_V_SCROLL_BAR_WIDTH + 1;
	rect.right = Bounds().right + 1;
	rect.bottom = Bounds().bottom - B_H_SCROLL_BAR_HEIGHT + 1;
	fVerScroller = new BScrollBar(rect, "", NULL, 0, 0, B_VERTICAL);
	rect.left = Bounds().left;
	rect.right = Bounds().right - B_V_SCROLL_BAR_WIDTH + 1;
	rect.bottom = Bounds().bottom + 1;
	rect.top = Bounds().bottom - B_H_SCROLL_BAR_HEIGHT + 1;
	fHorScroller = new BScrollBar(rect, "", NULL, 0, 0, B_HORIZONTAL);
	
	AddChild(fHorScroller);
	AddChild(fVerScroller);
	
	fHorScroller->SetRange(0,0);
	fVerScroller->SetRange(0,0);
		
	// init fFatBastard...
	fFatBastard =
		new BFIV_Tiler(new BMessenger(fStatus), ((BFIV_App *)be_app)->ScratchDirectory());
	
	// nothing here yet
	fImage = NULL;
	fScroller = NULL;
	// no reader thread yet
	fReaderThid = -1;
	// 2 ^ 0 = 100%
	fCurScale = 0;
	
	SetSizeLimits(199.0, 199.0, 99.0, 99.0);
}

BMenuBar * BFIV_Window::BuildMenus()
{
	BMessage *model;
	BMenuBar *menubar = new BMenuBar(Bounds(), "menubar"); // menubar
	BMenu *mainmenu;
	
	// the 'BFIV' menu
	mainmenu = new BMenu("BFIV");
	
	// add a couple items to our main menu
	mainmenu->AddItem(
		new BMenuItem("About", new BMessage(B_ABOUT_REQUESTED), 'A', B_COMMAND_KEY));
	mainmenu->AddItem(new BMenuItem("Image Stats", new BMessage(SHOW_STATS)));
	mainmenu->AddItem(new BMenuItem("Preferences"B_UTF8_ELLIPSIS, new BMessage(SHOW_PREFS_WINDOW)));
	mainmenu->AddItem(new BSeparatorItem());
	mainmenu->AddItem(
		new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q', B_COMMAND_KEY));
	// target be_app with the "About" item
	((BMenuItem *)mainmenu->ItemAt(0))->SetTarget(be_app);
	((BMenuItem *)mainmenu->ItemAt(4))->SetTarget(be_app);
	
	// the scale menu
	fScaleMenu = new BMenu("Scale");
	
	// we want radio mode
	fScaleMenu->SetRadioMode(true);
	
	// add items to the menu	
	model = new BMessage(SET_IMAGE_SCALE);
	model->AddInt32("incr", -1);
	fScaleMenu->AddItem(new BMenuItem("Down One", model, '-', B_COMMAND_KEY));
	
	model = new BMessage(SET_IMAGE_SCALE);
	model->AddInt32("incr", 1);
	fScaleMenu->AddItem(new BMenuItem("Up One", model, '+', B_COMMAND_KEY));
	
	fScaleMenu->AddItem(new BSeparatorItem());
	
	model = new BMessage(SET_IMAGE_SCALE);
	model->AddInt32("scale", -4);
	fScaleMenu->AddItem(new BMenuItem("6.25%", model));

	model = new BMessage(SET_IMAGE_SCALE);
	model->AddInt32("scale", -3);
	fScaleMenu->AddItem(new BMenuItem("12.5%", model));
	
	model = new BMessage(SET_IMAGE_SCALE);
	model->AddInt32("scale", -2);
	fScaleMenu->AddItem(new BMenuItem("25%", model));
	
	model = new BMessage(SET_IMAGE_SCALE);
	model->AddInt32("scale", -1);
	fScaleMenu->AddItem(new BMenuItem("50%", model));
	
	model = new BMessage(SET_IMAGE_SCALE);
	model->AddInt32("scale", 0);
	fScaleMenu->AddItem(new BMenuItem("100%", model));
	
	model = new BMessage(SET_IMAGE_SCALE);
	model->AddInt32("scale", 1);
	fScaleMenu->AddItem(new BMenuItem("200%", model));
	
	model = new BMessage(SET_IMAGE_SCALE);
	model->AddInt32("scale", 2);
	fScaleMenu->AddItem(new BMenuItem("400%", model));
	
	model = new BMessage(SET_IMAGE_SCALE);
	model->AddInt32("scale", 3);
	fScaleMenu->AddItem(new BMenuItem("800%", model));
	
	model = new BMessage(SET_IMAGE_SCALE);
	model->AddInt32("scale", 4);
	fScaleMenu->AddItem(new BMenuItem("1600%", model));
	
	model = NULL;
	// mark the 100% scale item
	fScaleMenu->FindItem("100%")->SetMarked(true);
	// add the menus to the menubar
	menubar->AddItem(mainmenu);
	menubar->AddItem(fScaleMenu);
	
	return menubar;
}

void BFIV_Window::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case B_MOUSE_WHEEL_CHANGED:
		{
			fScroller->MessageReceived(msg);
			break;
		}
		case SET_IMAGE_SCALE:
		{
			// first off, do we even have an image to scale?
			if(fImage != NULL)
			{
				int32 increment, scale;
				float fscale = 2.0;
				
				if(msg->FindInt32("scale", &scale) == B_OK)
				{
					fscale = pow(fscale, double(scale));
					fCurScale = scale;
					fImage->SetImageScale(fscale);
					fScroller->DoScrollBars();
				}
				else if(msg->FindInt32("incr", &increment) == B_OK)
				{
					if(increment > 0)
					{
						fscale = 2.0*fImage->ImageScale(); // multiply
						if(fscale <= 16.0)
							fCurScale++;
					}
					else
					{
						fscale = fImage->ImageScale()/2.0; // divide
						if(fscale >= (1.0/16.0))
							fCurScale--;
					}
					
					// always mark the current scale
					fScaleMenu->ItemAt(
						fScaleMenu->IndexOf(
							fScaleMenu->FindItem("100%")) + fCurScale)->SetMarked(true);
					
					// quick sanity check
					if(fscale >= (1.0/16.0) && fscale <= 16.0)
					{
						fImage->SetImageScale(fscale);
						fScroller->DoScrollBars();
					}
				}
			}
			else
			{
				// no scale change. Mark 100%
				fScaleMenu->ItemAt(
						fScaleMenu->IndexOf(fScaleMenu->FindItem("100%")))->SetMarked(true);
			}
			break;
		}
		case B_SIMPLE_DATA:
		{
			// dragged messages
			if(fImage == NULL) // no image open yet?
			{
				if(msg->FindRef("refs", &fFile_ref) == B_OK)
				{
					// spawn & resume the Open() thread				
					fReaderThid = spawn_thread(OpenThread, "File Reader", B_NORMAL_PRIORITY, (void *)this);
					resume_thread(fReaderThid);
				}
				
				// forward to be_app if there's more than one ref
				if(msg->HasRef("refs", 1))
				{
					msg->RemoveData("refs", 0); // don't wanna open this file twice
					msg->what = B_REFS_RECEIVED;
					be_app->PostMessage(msg);
				}
			}
			else // we already have an image thank you
			{
				// forward to be_app
				msg->what = B_REFS_RECEIVED;
				be_app->PostMessage(msg);
			}
			break;
		}
		case SHOW_STATS:
		{
			if(fFatBastard != NULL)
			{
				BString stats;
				
				stats << "Filename: " << ((fImage == NULL) ? "<none>": Title()) << '\n';
				
				fFatBastard->DumpStats(stats);
				(new BAlert("stats", stats.String(),"OK",
					0,0,B_WIDTH_AS_USUAL, B_IDEA_ALERT))->Go(NULL);
			}
			break;
		}
		case IMAGE_DONE_TRANSLATING:
		{
			// msg contents:
			// "err" - int32 - the return value of Translate()
			status_t err;
			const char *filename;
			
			if (msg->FindInt32("err", &err) == B_OK)
			{
				// check the value of err
				if (err == B_NO_ERROR)
				{
					if (msg->FindString("filename", &filename) == B_OK)
					{
						float menubottom = fStatus->Frame().top;
						BRect rect;
						
						// remove fStatus and delete it
						// init fImage and add it as a child
						RemoveChild(fStatus);
						delete fStatus;
						fStatus = NULL;
						
						// construct the container view for the image
						fImage = new BFIV_View("fatBastard", fFatBastard);
						
						// build the frame rect
						rect = Bounds();
						rect.top = menubottom;
						// construct the scrolling view
						fScroller = new BFIV_ScrollView(rect, fImage);
						// add the scrolling view to the window
						AddChild(fScroller);
						
						// set up the scrollbars
						fVerScroller->SetTarget(fScroller);
						fHorScroller->SetTarget(fScroller);
						
						// ok, you can resize it now...
						SetSizeLimits(200.0, 1000000.0, 100.0, 1000000.0);
						
						// and set the title of the window to something interesting
						SetTitle(filename);
						// scale image and set size of window here
						//fImage->SetImageScale(0.0625);
					}
				}
				else
				{
					// display an error alert.
					// reset fStatus
					(new BAlert("error",
					"An error was encountered while opening the image",
					"Darn"))->Go(NULL); // asynchronous alert
					
					// fire and forget message to fStatus
					BMessage msg(RESET_STATUS_BAR);
					BMessenger(fStatus).SendMessage(&msg);
				}
			}
			
			break;
		}
		case SHOW_PREFS_WINDOW:
		{
			(new BFIV_PrefWin(BRect(150,150, 449,224)))->Show();
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}

bool BFIV_Window::QuitRequested()
{
	// kill our file reader thread (if it's still running)
	if(fReaderThid > 0)
		kill_thread(fReaderThid);
	// clean up if a view isn't going to do it for us
	if(fImage == NULL)
		delete fFatBastard;
	// tell be_app to quit
	be_app->PostMessage(WINDOW_REGISTRY_SUB);
	return true;
}

int32 BFIV_Window::OpenThread(void *data)
{
	return ((BFIV_Window *)data)->Open();
}

int32 BFIV_Window::Open()
{
	// open the file
	// make a messenger to fStatus
	
	// opening the file...
	BFile *thePicture = new BFile(&fFile_ref, B_READ_ONLY);
	
	// good file ?
	if (thePicture->InitCheck() == B_OK)
	{
		status_t err;
		
		// tell fStatus to display its status bar
		BMessage msg(SHOW_STATUS_BAR);
		BEntry entry(&fFile_ref);
		char entryname[B_FILE_NAME_LENGTH];
		BTranslatorRoster *r = BTranslatorRoster::Default();
		translator_info t_info;
		
		entry.GetName(entryname); // leaf name, not full path
		msg.AddString("filename", entryname); // stick it in the msg
		BMessenger(fStatus).SendMessage(&msg); // fire it off to fStatus
		
		// Translate... takes a while to return on big images
		printf("Calling Identify()\n");
		err = r->Identify(thePicture, NULL, &t_info, 0, "image", B_TRANSLATOR_BITMAP);		
		if(err >= B_OK)
		{
			printf("Calling Translate()\n");
			err = r->Translate(t_info.translator,thePicture, NULL, fFatBastard, B_TRANSLATOR_BITMAP);
		}
		
		// tell this window that we're done translating the image.
		// window takes action based on the value of err
		msg.what = IMAGE_DONE_TRANSLATING;
		msg.AddInt32("err", err);
		PostMessage(&msg);
	}
	else
	{
		// we didn't open an image because the file was bad
		// reset fFatBastard to maintain a nice state
		fFatBastard->Reset();
	}
	
	// clean up
	delete thePicture;
	
	// invalidate our thid
	fReaderThid = -1;

	// die peacefully
	return B_OK;
}