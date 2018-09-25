/*
	
	ChangePassView.h
	John Wiggins 1999

*/

#ifndef CHANGE_PASS_VIEW_H
#define CHANGE_PASS_VIEW_H

#include <View.h>
#include <Button.h>
#include <Message.h>

#include "PassControl.h"

class ChangePassView : public BView {

public:
				ChangePassView(BRect frame, char *name); 
virtual	void	AttachedToWindow();

};

#endif
