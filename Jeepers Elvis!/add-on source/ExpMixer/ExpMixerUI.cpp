/*
	ExpMixerUI.cpp
	2000 John Wiggins
*/

#include "ExpMixerUI.h"

ExpMixerUI::ExpMixerUI(BRect frame, const char *name, ExpMix *mix)
: BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	BMessage *msg;
	BRect rect;
	
	// build a BStringView
	rect.Set(5.0,5.0, frame.Width()/2.0, 20.0);
	AddChild(new BStringView(rect, "string", "Background:"));
	
	// build our radio buttons
	// source 1
	rect.Set(rect.left, rect.bottom + 5.0, rect.right, rect.bottom + 20.0);
	msg = new BMessage(EM_SET_BACKGROUND);
	msg->AddInt32("which", 0);
	AddChild(radio1 = new BRadioButton(rect, "radio1", "Source 1", msg));
	// source 2
	rect.Set(rect.left, rect.bottom + 5.0, rect.right, rect.bottom + 20.0);
	msg = new BMessage(EM_SET_BACKGROUND);
	msg->AddInt32("which", 1);
	AddChild(radio2 = new BRadioButton(rect, "radio2", "Source 2", msg));
	// turn radio1 on
	radio1->SetValue(1);
	
	// build our op menu
	BPopUpMenu *menu = new BPopUpMenu("op menu");
	// add op
	msg = new BMessage(EM_SET_OP);
	msg->AddInt32("which", 0);
	menu->AddItem(new BMenuItem("add", msg));
	// sub op
	msg = new BMessage(EM_SET_OP);
	msg->AddInt32("which", 1);
	menu->AddItem(new BMenuItem("sub", msg));
	// max op
	msg = new BMessage(EM_SET_OP);
	msg->AddInt32("which", 2);
	menu->AddItem(new BMenuItem("max", msg));
	// min op
	msg = new BMessage(EM_SET_OP);
	msg->AddInt32("which", 3);
	menu->AddItem(new BMenuItem("min", msg));
	// now add it to a menu field
	rect.Set(frame.Width()/2. + 20., 25., frame.Width() - 10., 50.);
	AddChild(opMenu = new BMenuField(rect, "menu", "Op: ", menu));
	
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

ExpMixerUI::~ExpMixerUI()
{
}

void ExpMixerUI::GetPreferredSize(float *w, float *h)
{
	*w = width;
	*h = height;
}

void ExpMixerUI::AttachedToWindow()
{
	//printf("ChromaMixerUI::AttachedToWindow()\n");
	radio1->SetTarget(this);
	radio2->SetTarget(this);
	opMenu->Menu()->SetTargetForItems(this);
	opMenu->Menu()->ItemAt(0)->SetMarked(true);
}

void ExpMixerUI::DetachedFromWindow()
{
	//printf("ChromaMixerUI::DetachedFromWindow()\n");
}

void ExpMixerUI::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case EM_SET_BACKGROUND:
		{
			int32 which;
			
			if (msg->FindInt32("which", &which) == B_OK)
				backWhich = which;
			
			break;
		}
		case EM_SET_OP:
		{
			t_mix_op op = EXP_MIX_ADD;;
			int32 which;
			
			if (msg->FindInt32("which", &which) == B_OK)
			{
				switch(which)
				{
					case 0:
					{
						op = EXP_MIX_ADD;
						break;
					}
					case 1:
					{
						op = EXP_MIX_SUB;
						break;
					}
					case 2:
					{
						op = EXP_MIX_MAX;
						break;
					}
					case 3:
					{
						op = EXP_MIX_MIN;
						break;
					}
					default:
					{
						break;
					}
				}
			}
			
			theMixer->SetMixOp(op);
			break;
		}
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}

int32 ExpMixerUI::GetBackground() const
{
	return backWhich;
}

