/*
	
	ChangePassWindow.h
	John Wiggins 1999

*/

#ifndef CHANGE_PASS_WINDOW_H
#define CHANGE_PASS_WINDOW_H
// interface kit
#include <Window.h>
#include <Rect.h>
#include <Alert.h>
// app kit
#include <Application.h>
#include <Message.h>
// posix
#include <string.h>

#include "BCPMessages.h"
#include "ChangePassView.h"
#include "PassControl.h"


class ChangePassWindow : public BWindow {

public:
				ChangePassWindow(BRect frame); 
virtual	bool	QuitRequested();
virtual	void	MessageReceived(BMessage *msg);
};

#endif
