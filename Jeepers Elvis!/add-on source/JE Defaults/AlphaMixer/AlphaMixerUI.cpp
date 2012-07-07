/*
	AlphaMixerUI.cpp
	2000 John Wiggins
*/

#include "AlphaMixerUI.h"

AlphaMixerUI::AlphaMixerUI(BRect frame, const char *name, SuperMix *mix)
: BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	BMessage *msg;
	BRect rect;
	
	// build a BStringView
	rect.Set(5.0,5.0, frame.Width()/3.0, 20.0);
	AddChild(new BStringView(rect, "string", "Background:"));
	
	// build our radio buttons
	// source 1
	rect.Set(rect.left, rect.bottom + 5.0, rect.right, rect.bottom + 20.0);
	msg = new BMessage(AM_SET_BACKGROUND);
	msg->AddInt32("which", 0);
	AddChild(radio1 = new BRadioButton(rect, "radio1", "Source 1", msg));
	// source 2
	rect.Set(rect.left, rect.bottom + 5.0, rect.right, rect.bottom + 20.0);
	msg = new BMessage(AM_SET_BACKGROUND);
	msg->AddInt32("which", 1);
	AddChild(radio2 = new BRadioButton(rect, "radio2", "Source 2", msg));
	// turn radio1 on
	radio1->SetValue(1);
	
	// build our BSlider
	rect.Set((frame.Width()/3.0)+10.0, 10.0, frame.Width()-5.0, frame.Height()-10.0);
	AddChild(alphaSlider = new FUCKING_BSlider(rect, "alpha", mix));
	alphaSlider->SetLimitLabels("0", "1");
	alphaSlider->SetValue(127);
	alphaSlider->SetHashMarks(B_HASH_MARKS_BOTH);
	alphaSlider->SetHashMarkCount(3);
	alphaSlider->SetSnoozeAmount(33 * 1000); // 33ms
	alphaSlider->SetModificationMessage(new BMessage(AM_SET_ALPHA));
	
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

AlphaMixerUI::~AlphaMixerUI()
{
}

void AlphaMixerUI::GetPreferredSize(float *w, float *h)
{
	*w = width;
	*h = height;
}

void AlphaMixerUI::AttachedToWindow()
{
	//printf("AlphaMixerUI::AttachedToWindow()\n");
	radio1->SetTarget(this);
	radio2->SetTarget(this);
	alphaSlider->SetTarget(this);
}

void AlphaMixerUI::DetachedFromWindow()
{
	//printf("AlphaMixerUI::DetachedFromWindow()\n");
}

void AlphaMixerUI::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case AM_SET_BACKGROUND:
		{
			int32 which;
			
			if (msg->FindInt32("which", &which) == B_OK)
				backWhich = which;
			
			break;
		}
		case AM_SET_ALPHA:
		{
			// integer
			theMixer->SetAlpha(alphaSlider->Value());
			break;
		}
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}

int32 AlphaMixerUI::GetBackground() const
{
	return backWhich;
}

///////////////////
// FUCKING_BSlider
///////////////////
AlphaMixerUI::FUCKING_BSlider::FUCKING_BSlider(BRect frame, const char *name, SuperMix *mix)
: BSlider(frame, name, "Alpha", NULL, 0, 255)
{
	mixer = mix;
}

void AlphaMixerUI::FUCKING_BSlider::SetValue(int32 v)
{
	BSlider::SetValue(v);
	mixer->SetAlpha((uint8)v);
	//printf("FUCKING_BSlider::SetValue()\n");
}
