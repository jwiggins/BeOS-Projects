/*
	
	BCPWindow.cpp
	John Wiggins 1999

*/

#include "BCPWindow.h"


BCPWindow::BCPWindow(BRect frame)
		: BWindow(frame, "BeCheckPoint", B_TITLED_WINDOW,B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	LoginView			*aView;
	BMenuBar			*the_menubar;
	BMenu				*the_menu;
	BRect				aRect;
	int32				menubarheight=0;
	
	// make yourself a happy little menu
	aRect.Set(0,0, 350, 5);
	the_menubar = new BMenuBar(aRect, "menubar");
	the_menu = new BMenu("BeCheckPoint");
	the_menu->AddItem(new BMenuItem("Change Password", new BMessage(CHANGE_PASSWORD_DIALOG), 'P', B_COMMAND_KEY));
	the_menu->AddItem(new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q', B_COMMAND_KEY));
	((BMenuItem *)the_menu->ItemAt(1))->SetTarget(be_app);
	
	the_menubar->AddItem(the_menu);
	
	// add the menubar to the window
	AddChild(the_menubar);
	
	// now get the height of the menubar 
	menubarheight = (int32)the_menubar->Bounds().Height();
	
	// set up a rectangle and instantiate a new view
	// view rect should be same size as window rect but with left top at (0, 0)
	aRect = Bounds();
	aRect.top += menubarheight;
	aView = new LoginView(aRect, "Login View");
	
	
	
	// add view to window
	AddChild(aView);
}

bool BCPWindow::QuitRequested()
{
	if(((BCPApp *)be_app)->ok_to_quit())
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return BWindow::QuitRequested();
	}
	else
		return false;
}

void BCPWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case BUTTON_PRESS:
			{
				BMessage hey(PASSWORD_ATTEMPT);
				const char *entered_password;
				PassControl	*pass = static_cast<PassControl *>(FindView("password"));
				
				// package up the password in the PassControl
				entered_password = pass->actualText();				
				hey.AddString("password", entered_password);
				
				// send it to be_app
				be_app->PostMessage(&hey);
				break;
			}
		case CHANGE_PASSWORD_DIALOG:
			{
				BRect rect;
				ChangePassWindow *win;
				
				rect.Set(300, 300, 599, 399);
				win = new ChangePassWindow(rect);
				
				win->Show();
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