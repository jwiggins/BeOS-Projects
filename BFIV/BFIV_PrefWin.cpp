#include "BFIV_PrefWin.h"

BFIV_PrefWin::BFIV_PrefWin(BRect frame)
: BWindow(frame, "Preferences", B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE|B_NOT_ZOOMABLE)
{
	BRect rect = Bounds();
	BView *back;
	BTextControl *scratchPath;
	BButton *cancelButton, *okButton, *selectButton;
	font_height fontHeight;
	float fontSize, tmp;
	
	// private data
	filePanel = NULL;
	
	// some font voodoo
	be_plain_font->GetHeight(&fontHeight);
	fontSize = fontHeight.ascent + fontHeight.descent;
	
	// the background
	back = new BView(rect, "background", B_FOLLOW_ALL, 0);
	back->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(back);
	
	// the select button
	tmp = be_plain_font->StringWidth("Select");
	rect.Set(frame.Width() - tmp - 20., 5., frame.Width() - 5., 5. + fontSize + 10.);
	selectButton = new BButton(rect, "select", "Select", new BMessage(SELECT_SCRATCH_PATH));
	back->AddChild(selectButton);
	
	// the path
	rect.Set(5.,5., selectButton->Frame().left - 5., 5. + fontSize + 10.);
	scratchPath = new BTextControl(rect, "path", "Scratch Location", NULL, NULL, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);
	back->AddChild(scratchPath);
	scratchPath->SetText(((BFIV_App *)be_app)->ScratchDirectory()); // current dir
	scratchPath->TextView()->MakeEditable(false); // only works after AddChild() (!!)
	
	
	// the ok button
	tmp = be_plain_font->StringWidth("OK");
	rect.Set(frame.Width() - tmp - 35., Frame().Height() - fontSize - 20., frame.Width() - 10., Frame().Height() - 10.);
	okButton = new BButton(rect, "ok", "OK", new BMessage(APPLY_PREFS_CHANGE));
	back->AddChild(okButton);
	okButton->MakeDefault(true);
	
	// the cancel button
	tmp = be_plain_font->StringWidth("Cancel");
	rect.Set(okButton->Frame().left - tmp - 20., okButton->Frame().top, okButton->Frame().left - 5., okButton->Frame().bottom);
	cancelButton = new BButton(rect, "cancel", "Cancel", new BMessage(B_QUIT_REQUESTED));
	back->AddChild(cancelButton);
}

BFIV_PrefWin::~BFIV_PrefWin()
{
	if(filePanel)
		delete filePanel;
}

bool BFIV_PrefWin::QuitRequested()
{
	return true;
}

void BFIV_PrefWin::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case SELECT_SCRATCH_PATH:
		{
			if(filePanel == NULL)
			{
				filePanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL,
													B_DIRECTORY_NODE, false, NULL, NULL, false);
			}
			filePanel->Show();
			break;
		}
		case B_REFS_RECEIVED:
		{
			entry_ref ref;
			
			// grab the ref
			if(msg->FindRef("refs", &ref) == B_OK)
			{
				BEntry entry(&ref); // make an entry out of it
				BPath path;
				
				if(entry.InitCheck() == B_OK) // does it exist ?
				{
					entry.GetPath(&path); // get the path for it
					((BTextControl *)FindView("path"))->SetText(path.Path()); // show it
				}
			}
			break;
		}
		case APPLY_PREFS_CHANGE:
		{
			// tell be_app about the new path for scratch files
			BMessage msg(APPLY_PREFS_CHANGE);
			
			msg.AddString("path", ((BTextControl *)FindView("path"))->Text()); // add the path
			be_app->PostMessage(&msg); // here ya go, be_app
			
			// close this window
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