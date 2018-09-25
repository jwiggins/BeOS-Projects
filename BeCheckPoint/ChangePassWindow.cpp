/*
	
	ChangePassWindow.cpp
	John Wiggins 1999

*/

#include "ChangePassWindow.h"


ChangePassWindow::ChangePassWindow(BRect frame)
		: BWindow(frame, "Change Password", B_TITLED_WINDOW,B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	BRect			rect = frame;
	ChangePassView	*view;
	
	rect.OffsetTo(B_ORIGIN);
	view = new ChangePassView(rect, "view");
	AddChild(view);
}

bool ChangePassWindow::QuitRequested()
{
	PostMessage(B_QUIT_REQUESTED);
	return BWindow::QuitRequested();
}

void ChangePassWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case BUTTON_PRESS:
			{
				BMessage hey(PASSWORD_CHANGE);
				const char *old_password, *new_password, *confirm_password;
				PassControl	*old_pass = static_cast<PassControl *>(FindView("old"));
				PassControl	*new_pass = static_cast<PassControl *>(FindView("new"));
				PassControl	*confirm_pass = static_cast<PassControl *>(FindView("confirm"));
				
				// package up the text in the PassControls
				old_password = old_pass->actualText();
				new_password = new_pass->actualText();
				confirm_password = confirm_pass->actualText();
				if(strcmp(new_password, confirm_password) == 0)
				{
					hey.AddString("oldpassword", old_password);
					hey.AddString("newpassword", new_password);
				
					// send it to be_app
					be_app->PostMessage(&hey);
				}
				else
					(new BAlert("", "The \'new\' and \'confirm\' feilds do not match", "ok"))->Go(NULL);
				break;
			}
		case SHAKE_WINDOW:
			{
				//msg->PrintToStream();
				MoveBy(50,0);
				snooze(80 * 1000);
				MoveBy(-50,0);
				snooze(80 * 1000);
				MoveBy(50,0);
				snooze(80 * 1000);
				MoveBy(-50,0);
				snooze(80 * 1000);
				break;
			}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}