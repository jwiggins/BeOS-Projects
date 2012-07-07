#include "BFIV_Status.h"

BFIV_Status::BFIV_Status(BRect frame, const char *name)
: BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	BRect rect;
	
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	rect.Set(5,(frame.Height()/2.0) - 10, frame.Width()-5, (frame.Height()/2.0) + 10);
	// the stringview
	fDrop = new BStringView(rect, "drop", "Drop a file on me!");
	fDrop->SetFontSize(18.0);
	fDrop->SetAlignment(B_ALIGN_CENTER);
	// the statusbar
	fStatus = new BStatusBar(rect, "statusbar", "Loading:", "% read");
	fStatus->SetBarHeight(12.0);
	fStatus->SetText("none");
	fStatus->SetTrailingText("0");
	
	AddChild(fDrop);
	AddChild(fStatus);
	
	fStatus->Hide();
}

void BFIV_Status::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case SHOW_STATUS_BAR:
		{
			// Hide() fDrop and Show() fStatus
			// Fields are:
			// "filename" : string : leaf name of the file being opened
			const char *name;
			
			// no more "Drop a file on me"
			fDrop->Hide();
			// add the file leaf name to the status bar
			if (msg->FindString("filename", &name) == B_OK)
				fStatus->SetText(name);
			else
				fStatus->SetText(B_EMPTY_STRING);
			// show the status bar
			fStatus->Show();
			break;
		}
		case IMAGE_STATUS_UPDATE:
		{
			// update fStatus
			// Fields are:
			// "delta" : float : number from 0.0 -> 1.0; percentage of image just written
			float delta;
			
			if(msg->FindFloat("delta", &delta) == B_OK)
			{
				char delt[8];
				float pctg;
				
				pctg = fStatus->CurrentValue() + fStatus->MaxValue()*delta;
				sprintf(delt, "%6.2f", pctg); // trailing text floating point #
				fStatus->Update(fStatus->MaxValue()*delta, NULL, delt);
			}
			
			break;
		}
		case RESET_STATUS_BAR:
		{
			// we're here because an image failed to load
			// reset the status bar
			// hide ourselves and show fDrop
			
			// switcheroo
			fStatus->Hide();
			fDrop->Show();
			
			fStatus->Reset("Loading:", "% read"); // same label and trailing label
			fStatus->SetText(B_EMPTY_STRING); // no file
			break;
		}
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}