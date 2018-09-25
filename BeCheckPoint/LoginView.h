/*
	
	LoginView.h
	John Wiggins 1999

*/

#ifndef LOGIN_VIEW_H
#define LOGIN_VIEW_H

// interface kit
#include <View.h>
#include <Button.h>
#include <Rect.h>
#include <Font.h>
// app kit
#include <Message.h>

#include "PassControl.h"

class LoginView : public BView {

public:
				LoginView(BRect frame, char *name); 
virtual	void	AttachedToWindow();

};

#endif
