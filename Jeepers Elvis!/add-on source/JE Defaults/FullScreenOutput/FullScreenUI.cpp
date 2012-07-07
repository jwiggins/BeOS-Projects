/*
	FullScreenUI.cpp
	2000 John Wiggins
	
	2002: fixed original code, changed UI, added overlay
*/

#include "FullScreenUI.h"

FullScreenUI::FullScreenUI(BRect frame, const char *name, fullscreen_output_private *data)
: BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	BRect rect;
	BMessage *msg;
	
	// view color
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	// cache width and height
	width = frame.Width();
	height = frame.Height();
	
	// build our resolution menu
	BPopUpMenu *menu = new BPopUpMenu("res menu");
	// 640x480x32bpp
	msg = new BMessage(FS_SWITCH_RESOLUTION);
	msg->AddInt32("which", 0);
	menu->AddItem(new BMenuItem("640x480x32bpp", msg));
	// 800x600x32bpp
	msg = new BMessage(FS_SWITCH_RESOLUTION);
	msg->AddInt32("which", 1);
	menu->AddItem(new BMenuItem("800x600x32bpp", msg));
	// 1024x768x32bpp
	msg = new BMessage(FS_SWITCH_RESOLUTION);
	msg->AddInt32("which", 2);
	menu->AddItem(new BMenuItem("1024x768x32bpp", msg));
	// now add it to a menu field (and to the view)
	rect.Set(5.0, 5.0, width-5.0, 30.0);
	AddChild(resMenu = new BMenuField(rect, "menu", "Resolution: ", menu));
	
	// a checkbox
	rect.Set(rect.left, rect.bottom+5.0, (width*4.0/7.0), rect.bottom+30.0);
	msg = new BMessage(FS_TOGGLE_OVERLAY);
	AddChild(olCheckBox = new BCheckBox(rect, "checkbox", "Use Overlay", msg));
	
	// a nice pretty button
	rect.Set(rect.right, rect.bottom+5.0, width-5.0, height-5.0);
	msg = new BMessage(FS_TOGGLE_FULL_SCREEN);
	AddChild(toggleButton = new BButton(rect, "button", "Start", msg));
	
	// the fullscreen window we control
	//vidWin = data->vidWin;
	// magic data cookie
	cookie = data;
}

void FullScreenUI::AttachedToWindow()
{
	toggleButton->SetTarget(this);
	olCheckBox->SetTarget(this);
	resMenu->Menu()->SetTargetForItems(this);
	resMenu->Menu()->ItemAt(0)->SetMarked(true);
	
	resMenu->SetDivider(resMenu->Divider()*2./3.);
	
	// select the first item (640x480)
//	resList->Select(0L);
//	resList->SetTarget(this);
}

void FullScreenUI::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case FS_TOGGLE_FULL_SCREEN:
		{
			//printf("vidWin->IsLocked() = %s\n", (vidWin->IsLocked() ? "true" : "false"));
			if (cookie->overlay)
			{
				// using the overlay window
				cookie->ovlWin->PostMessage(OO_VISIBILITY_TOGGLE);
			}
			else if (cookie->vidWin != NULL && cookie->vidWin->Lock())
			{
				// doing it the "ol' fashioned" way (the slow way) 
				if (cookie->vidWin->IsHidden())
				{
					cookie->vidWin->Show();
					cookie->vidWin->Unlock();
				}
			}
			
			break;
		}
		case FS_TOGGLE_OVERLAY:
		{
			// checked
			if (olCheckBox->Value() == 1)
			{
				// disable the resolution picker
				resMenu->SetEnabled(false);
				// set the overlay flag
				cookie->overlay = true;
			}
			else // unchecked
			{
				// re-enable the resolution picker
				resMenu->SetEnabled(true);
				// unset the overlay flag
				cookie->overlay = false;
			}
			
			break;
		}
		case FS_SWITCH_RESOLUTION:
		{
			//printf("FullScreenUI::MessageReceived(FS_SWITCH_RESOLUTION)\n");
			
			int32 which;
			status_t err;
			VideoScreen *nWin = NULL;
			
			if (msg->FindInt32("which", &which) == B_OK)
			{
				switch(which)
				{
					case 0: // switch to 640x480
					{
						// get rid of the current video screen
						if (cookie->vidWin != NULL)
						{
							cookie->vidWin->Lock();
							cookie->vidWin->Quit();
							
							cookie->vidWin = NULL;
						}
						
						// make a new VideoScreen
						nWin = new VideoScreen(B_32_BIT_640x480, &err);
						
						// check for bad construction
						if (err != B_OK)
						{
							(void)(new BAlert("",strerror(err), "Ok"))->Go();
							nWin->Lock();
							nWin->Quit();
							
							nWin = NULL;
						}
						else // successful construction
						{
							//vidWin = nWin;
							cookie->vidWin = nWin;
						}
						
						break;
					}
					case 1: // switch to 800x600
					{
						// get rid of the current video screen
						if (cookie->vidWin != NULL)
						{
							cookie->vidWin->Lock();
							cookie->vidWin->Quit();
							
							cookie->vidWin = NULL;
						}
									
						// make a new VideoScreen
						nWin = new VideoScreen(B_32_BIT_800x600, &err);
						
						// check for bad construction
						if (err != B_OK)
						{
							(void)(new BAlert("",strerror(err), "Ok"))->Go();
							nWin->Lock();
							nWin->Quit();
							
							nWin = NULL;
						}
						else // successful construction
						{
							//vidWin = nWin;
							cookie->vidWin = nWin;
						}
						
						break;
					}
					case 2: // switch to 1024x768
					{
						// get rid of the current video screen
						if (cookie->vidWin != NULL)
						{
							cookie->vidWin->Lock();
							cookie->vidWin->Quit();
							
							cookie->vidWin = NULL;
						}
						
						// make a new VideoScreen
						nWin = new VideoScreen(B_32_BIT_1024x768, &err);
						
						// check for bad construction
						if (err != B_OK)
						{
							(void)(new BAlert("",strerror(err), "Ok"))->Go();
							nWin->Lock();
							nWin->Quit();
							
							nWin = NULL;
						}
						else // successful construction
						{
							//vidWin = nWin;
							cookie->vidWin = nWin;
						}
	
						break;
					}
					default:
					{
						break;
					}
				}
			}
			
			break;
		}
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}

void FullScreenUI::GetPreferredSize(float *w, float *h)
{
	*w = width;
	*h = height;
}
