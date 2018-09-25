/*
	
	LoginView.cpp
	John Wiggins 1999

*/

#include "LoginView.h"
#include "BCPMessages.h"

LoginView::LoginView(BRect rect, char *name)
	   	   : BView(rect, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetHighColor(255,255,0);
	SetViewColor(220,220,220);
}


void LoginView::AttachedToWindow()
{
	PassControl		*password; // my speshul Password masking BTextControl.
	BButton			*enter;
	BRect			rect, bounds = Bounds();
	int32			interfaceUnit=0;
	
	interfaceUnit = ((int32)be_fixed_font->Size());
	
	rect.Set(interfaceUnit, interfaceUnit<<1, (bounds.right - StringWidth("enter")) - (interfaceUnit*3), interfaceUnit*3);
	
	password = new PassControl(rect, "password", "password", NULL, new BMessage(DUMMY));
	AddChild(password);
	
	password->MakeFocus();
	
	rect.Set((bounds.right - StringWidth("enter")) - (interfaceUnit<<1), interfaceUnit<<1, (bounds.right - interfaceUnit), interfaceUnit*3);
	enter = new BButton(rect, "enter", "enter", new BMessage(BUTTON_PRESS));
	enter->MakeDefault(true);
	AddChild(enter);
}
