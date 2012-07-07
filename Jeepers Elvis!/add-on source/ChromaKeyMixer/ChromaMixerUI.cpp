/*
	ChromaMixerUI.cpp
	2000 John Wiggins
*/

#include "ChromaMixerUI.h"

ChromaMixerUI::ChromaMixerUI(BRect frame, const char *name, ChromaMix *mix)
: BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	BMessage *msg;
	BRect rect;
	
	// build a BStringView
	rect.Set(5.0,5.0, frame.Width()/4.0, 20.0);
	AddChild(new BStringView(rect, "string", "Background:"));
	
	// build our radio buttons
	// source 1
	rect.Set(rect.left, rect.bottom + 5.0, rect.right, rect.bottom + 20.0);
	msg = new BMessage(CM_SET_BACKGROUND);
	msg->AddInt32("which", 0);
	AddChild(radio1 = new BRadioButton(rect, "radio1", "Source 1", msg));
	// source 2
	rect.Set(rect.left, rect.bottom + 5.0, rect.right, rect.bottom + 20.0);
	msg = new BMessage(CM_SET_BACKGROUND);
	msg->AddInt32("which", 1);
	AddChild(radio2 = new BRadioButton(rect, "radio2", "Source 2", msg));
	// turn radio1 on
	radio1->SetValue(1);
	
	// build our BColorControl
	rect.Set((frame.Width()/4.0)+10.0, 10.0, frame.Width()-5.0, frame.Height()-10.0);
	msg = new BMessage(CM_SET_KEY);
	AddChild(
		keyColor = new BColorControl(rect.LeftTop(), B_CELLS_32x8, 3.0f, "keycolor", msg));
	rgb_color blue = {0,0,255,0};
	keyColor->SetValue(blue);
	
	// cache our initial width and height
	width = frame.Width();
	height = frame.Height();
	
	// background defaults to 0 (source 1)
	backWhich = 0;
	
	// know thy mixer
	theMixer = mix;
	
	// set the view color
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

ChromaMixerUI::~ChromaMixerUI()
{
}

void ChromaMixerUI::GetPreferredSize(float *w, float *h)
{
	*w = width;
	*h = height;
}

void ChromaMixerUI::AttachedToWindow()
{
	//printf("ChromaMixerUI::AttachedToWindow()\n");
	radio1->SetTarget(this);
	radio2->SetTarget(this);
	keyColor->SetTarget(this);
}

void ChromaMixerUI::DetachedFromWindow()
{
	//printf("ChromaMixerUI::DetachedFromWindow()\n");
}

void ChromaMixerUI::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case CM_SET_BACKGROUND:
		{
			int32 which;
			
			if (msg->FindInt32("which", &which) == B_OK)
				backWhich = which;
			
			break;
		}
		case CM_SET_KEY:
		{
			theMixer->SetKey(keyColor->ValueAsColor());
			break;
		}
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}

int32 ChromaMixerUI::GetBackground() const
{
	return backWhich;
}

