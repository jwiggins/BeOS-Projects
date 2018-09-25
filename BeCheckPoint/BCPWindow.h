/*
	
	BCPWindow.h
	John Wiggins 1999

*/

#ifndef BCP_WINDOW_H
#define BCP_WINDOW_H

// interface kit
#include <Window.h>
#include <Rect.h>
// app kit
#include <Application.h>
#include <Message.h>
// posix
#include <string.h>

#include "main.h"
#include "LoginView.h"
#include "BCPMessages.h"
#include "ChangePassWindow.h"
#include "PassControl.h"


class BCPWindow : public BWindow {

public:
				BCPWindow(BRect frame); 
virtual	bool	QuitRequested();
virtual	void	MessageReceived(BMessage *msg);
};

#endif
