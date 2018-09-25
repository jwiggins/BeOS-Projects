/*
	
	main.h
	John Wiggins 1999

*/

#ifndef MAIN_H
#define MAIN_H

// app kit
#include <Application.h>
#include <Message.h>
#include <Messenger.h>
#include <Roster.h>
// interface kit
#include <Alert.h>
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
// storage kit
#include <File.h>
#include <Path.h>
#include <FindDirectory.h>
// posix
#include <string.h>
#include <stdio.h>

#include "BCPWindow.h"
#include "BCPMessages.h"

class BCPApp : public BApplication {

public:
					BCPApp();
virtual	void		MessageReceived(BMessage *msg);
virtual	bool		QuitRequested();
virtual	void		ReadyToRun();
BWindow	*			FindWindow(const char *title);
void				kill_apps();
void				unkill_apps(); // :P
void				set_pass(const char *pass);
status_t			read_pass();
status_t			write_pass(const char *pass);
status_t			change_pass(const char *old, const char *_new);
bool				ok_to_quit() { return success; };

private:
char				*_password;
// booleans
bool				mail_daemon, success;
};

#endif
