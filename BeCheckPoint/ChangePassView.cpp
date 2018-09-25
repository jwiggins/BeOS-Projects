/*
	
	ChangePassView.cpp
	John Wiggins 1999

*/

#include "ChangePassView.h"
#include "BCPMessages.h"

ChangePassView::ChangePassView(BRect rect, char *name)
	   	   : BView(rect, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetHighColor(255,255,0);
	SetViewColor(220,220,220);
}


void ChangePassView::AttachedToWindow()
{
	PassControl		*old, *_new, *confirm; // my speshul Password masking BTextControl.
	BButton			*enter;
	BRect			rect, bounds = Bounds();
	int32			interfaceUnit=0;
	
	interfaceUnit = ((int32)be_fixed_font->Size());
	
	rect.Set(interfaceUnit, interfaceUnit, (bounds.right - StringWidth("enter")) - (interfaceUnit*3), interfaceUnit*3);
	old = new PassControl(rect, "old", "old", NULL, new BMessage(DUMMY));
	AddChild(old);
	old->MakeFocus();
	
	rect.Set(interfaceUnit, interfaceUnit*3, (bounds.right - StringWidth("enter")) - (interfaceUnit*3), interfaceUnit<<2);
	_new = new PassControl(rect, "new", "new", NULL, new BMessage(DUMMY));
	AddChild(_new);
	
	rect.Set(interfaceUnit, interfaceUnit*5, (bounds.right - StringWidth("enter")) - (interfaceUnit*3), interfaceUnit*7);
	confirm = new PassControl(rect, "confirm", "confirm", NULL, new BMessage(DUMMY));
	AddChild(confirm);
	
	rect.Set((bounds.right - StringWidth("enter")) - (interfaceUnit<<1), interfaceUnit*5, (bounds.right - interfaceUnit), interfaceUnit*6);
	enter = new BButton(rect, "enter", "enter", new BMessage(BUTTON_PRESS));
	enter->MakeDefault(true);
	AddChild(enter);
}
