/*
	
	main.cpp
	John Wiggins 1999

*/

#include "main.h"

main()
{	
	BCPApp *myApplication;

	myApplication = new BCPApp();
	myApplication->Run();
	
	delete(myApplication);
	return(0);
}

// most of this constructor was stolen from Be's old HelloWorld example

BCPApp::BCPApp()
		  	: BApplication("application/x-vnd.Prok-BeCheckPoint")
{
	BCPWindow			*aWindow;
	BRect				aRect;
		
	// generic inits for other members of the class
	_password = NULL;
	mail_daemon = false;
	success = false;

	// set up a rectangle and instantiate a new window
	aRect.Set(249, 249, 599, 349);
	aWindow = new BCPWindow(aRect);
	
	// make window visible
	aWindow->Show();
}

void BCPApp::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case PASSWORD_ATTEMPT:
			{
				const char *attempted_password;
				
				if(msg->FindString("password", &attempted_password) == B_NO_ERROR)
				{
					//msg->PrintToStream();
					//printf("password: %s\n", _password);
					if(!strcmp(attempted_password, _password))
					{
						// they passed. lets get out of here.
						success = true;
						PostMessage(B_QUIT_REQUESTED);
					}
					else
					{
						//printf("wrong password. sending SHAKE_WINDOW BMessage\n");
						FindWindow("BeCheckPoint")->PostMessage(SHAKE_WINDOW);
					}
				}
			}
			break;
		case PASSWORD_CHANGE:
			{
				const char *old_password, *new_password;
				
				if(msg->FindString("oldpassword", &old_password) == B_NO_ERROR)
				{
					if(msg->FindString("newpassword", &new_password) == B_NO_ERROR)
					{
						//msg->PrintToStream();
						//printf("password: %s\n", _password);
						status_t err;
						err = change_pass(old_password, new_password);
						if((err < B_NO_ERROR) && (err != WRONG_PASSWORD))
							(new BAlert("alert","There was an error changing your password.","ok"))->Go();
						else if(err == WRONG_PASSWORD)
						{
							FindWindow("Change Password")->PostMessage(SHAKE_WINDOW);
						}
						else
						{
							// kill the change password window
							FindWindow("Change Password")->PostMessage(B_QUIT_REQUESTED);
						}
					}
				}
			}
			break;
		default:
			BApplication::MessageReceived(msg);
			break;
	}
}

bool BCPApp::QuitRequested()
{
	if(success)
	{
		status_t err;
		err = write_pass(_password);
		if(err < B_NO_ERROR)
			(new BAlert("bad write","An error occurred while writing your password to disk.\nAny changes you might have made will not be saved.\n","ok"))->Go();
		unkill_apps();
		return true;
	}
	else
		return false;
}

void BCPApp::ReadyToRun()
{
	status_t	err;
	if((err = read_pass()) < B_NO_ERROR) // inits _password
	{
		// there was an error
		if(err == B_ENTRY_NOT_FOUND)
		{
			// pop up an Alert telling the user that the password is
			// defaulting to "none"
			(new BAlert("default password",
						"Password File not Found.\nYour Password has been set to the default: \"none\"",
						"ok"))->Go();
			set_pass("none");
			//printf("set_pass(). password: %s\n", _password);
		}
		else
		{
			// show what the error was
			char message[1024];
			sprintf(message, "There was an error while reading the password:\n %s\n Setting your Password to the Default: \"none\"", strerror(err));
			(new BAlert("didnt read password", message ,"ok"))->Go();
			set_pass("none");
			//printf("set_pass(). password: %s\n", _password);
		}
		kill_apps();
	}
	else
	{
		kill_apps();
	}
	//printf("end of ReadyToRun(). password: %s\n", _password);
}

BWindow * BCPApp::FindWindow(const char *title)
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

void BCPApp::kill_apps()
{
	// as fun to name as they are to use >:)
	BMessenger	postal_mail_employee("application/x-vnd.Be-POST");
	BMessenger	start_menu_wannabe("application/x-vnd.Be-TSKB");
	BMessenger	desktops_are_passe("application/x-vnd.Be-TRAK");
	
	if(postal_mail_employee.IsValid())
	{
		mail_daemon = true;
		postal_mail_employee.SendMessage(B_QUIT_REQUESTED);
		snooze(30 * 1000);
		// check on it, and kill it if it's not dead
		if(postal_mail_employee.IsValid())
			kill_team(postal_mail_employee.Team()); 
	}
	if(start_menu_wannabe.IsValid())
	{
		start_menu_wannabe.SendMessage(B_QUIT_REQUESTED);
		snooze(30 * 1000);
		if(start_menu_wannabe.IsValid())
			kill_team(start_menu_wannabe.Team());
	}
	if(desktops_are_passe.IsValid())
	{
		desktops_are_passe.SendMessage(B_QUIT_REQUESTED);
		snooze(30 * 1000);
		if(desktops_are_passe.IsValid())
			kill_team(desktops_are_passe.Team());
	}
}

void BCPApp::unkill_apps()
{
	be_roster->Launch("application/x-vnd.Be-TRAK");
	be_roster->Launch("application/x-vnd.Be-TSKB");
	if(mail_daemon)
	{
		snooze(30 * 1000); // chill for a second
		be_roster->Launch("application/x-vnd.Be-POST");
	}
}

status_t BCPApp::read_pass()
{
	status_t	err=B_ERROR;
	BFile		file;
	BPath		path;
	char		*temp;
	off_t		size=0;
	
	if((err = find_directory(B_USER_SETTINGS_DIRECTORY, &path)) < B_NO_ERROR)
		return err;
	if((err = path.Append("BeCheckPoint")) < B_NO_ERROR)
		return err;
	if((err = file.SetTo(path.Path(), B_READ_ONLY)) < B_NO_ERROR)
		return err;	
	if((err = file.GetSize(&size)) < B_NO_ERROR || (size == 0))
		return err;
	// semi-kludgy workaround for avoiding password corruption in memory
	temp = new char [ size + 1 ];
	if((err = file.ReadAt(0, (void *)temp, size)) < B_NO_ERROR)
	{
		_password = NULL;
		return err;
	}
	_password = new char [ strlen(temp) + 1 ];
	strcpy(_password, temp);
	
	delete [] temp;
	
	return B_NO_ERROR; // we made it.
}

status_t BCPApp::write_pass(const char *pass)
{
	//printf("write_pass(). password: %s; writing: %s\n", _password, pass);
	status_t	err=B_ERROR;
	BFile		file;
	BPath		path;
	
	err = find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeCheckPoint");
	
	err = file.SetTo(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	
	err = file.WriteAt(0, (void *)pass, strlen(pass)+1);
	
	return err;
}

status_t BCPApp::change_pass(const char *old, const char *_new)
{
	if(!strcmp(old, _password)) // match
	{
		set_pass(_new); // change the password in memory
		return write_pass(_new); // write the new one to disk
	}
	
	return WRONG_PASSWORD;
}

void BCPApp::set_pass(const char *pass)
{
	if(_password == NULL)
	{
		_password = new char [ strlen(pass) + 1 ];
		strcpy(_password, pass);
	}
	else
	{
		delete [] _password;
		_password = new char [ strlen(pass) + 1 ];
		strcpy(_password, pass);
	}
}