#include "BFIV_App.h"

BFIV_App::BFIV_App(const char *sig)
: BApplication(sig)
{
	fWinCount = 0;
	
	// read the settings
	BPath path;
	BFile settings;
	
	// find the user's settings directory
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	
	// append BFIV
	path.Append("BFIV");
	
	// open file
	settings.SetTo(path.Path(), B_READ_ONLY);
	
	// check it
	if(settings.InitCheck() == B_OK)
	{
		char pathname[B_PATH_NAME_LENGTH];
		
		memset((void *)pathname, 0, B_PATH_NAME_LENGTH);
		settings.ReadAttr("path", B_STRING_TYPE, 0, (void *)pathname, B_PATH_NAME_LENGTH - 1);
		pathname[B_PATH_NAME_LENGTH] = 0;
		
		fScratchDir = pathname;
	}
	else
	{
		fScratchDir = "/tmp";
	}
}

void BFIV_App::ReadyToRun()
{
	// show a window if we haven't got any windows open already
	if(CountWindows() < 1)
		(new BFIV_Window(BRect(100,100, 299, 199), "BFIV"))->Show();
}

void BFIV_App::RefsReceived(BMessage *msg)
{
	BFIV_Window *aWindow;
	entry_ref file_ref;
	int32 index = 0;
	
	//printf("Got a ref\n");
	//(new BAlert("", "Got a ref", "ok"))->Go(NULL);
	
	while (msg->FindRef("refs", index++, &file_ref) == B_OK)
	{
		// an open message
		BMessage open_msg(B_SIMPLE_DATA);
		open_msg.AddRef("refs", &file_ref);
		
		// build a window
		aWindow = new BFIV_Window(BRect(100,100, 299, 199), "BFIV");		
		// send it an open message
		aWindow->PostMessage(&open_msg);
		// show it
		aWindow->Show();
	}
}

void BFIV_App::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case WINDOW_REGISTRY_ADD:
		{
			fWinCount++;
			break;
		}
		case WINDOW_REGISTRY_SUB:
		{
			fWinCount--;
			if(fWinCount == 0)
				PostMessage(B_QUIT_REQUESTED);
			break;
		}
		case APPLY_PREFS_CHANGE:
		{
			const char *path;
			
			if (msg->FindString("path", &path) == B_OK)
			{
				// set fScratchDir to the new path
				fScratchDir = path;
			}
			break;
		}
		default:
		{
			BApplication::MessageReceived(msg);
			break;
		}
	}
}

void BFIV_App::AboutRequested()
{
	(new BAlert("",
	"BFIV v1.0\n"
	"1999 John Wiggins\n", "OK"))->Go(NULL);
}

bool BFIV_App::QuitRequested()
{
	// save scratch directory to settings file
	BPath path;
	BFile settings;
	
	// find the user's settings directory
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	
	// append BFIV
	path.Append("BFIV");
	
	// open file
	settings.SetTo(path.Path(), B_WRITE_ONLY | B_CREATE_FILE);
	
	// check it
	if(settings.InitCheck() == B_OK)
	{
		// write
		settings.WriteAttr("path", B_STRING_TYPE, 0,
			(void *)fScratchDir.String(), fScratchDir.Length());
	}
	
	return true;
}

int main()
{
	BFIV_App app("application/x-vnd.prok-BFIV");
	
	app.Run();
	
	return B_OK;
}
