/*
	ChannelWhackerUI.cpp
	2000 John Wiggins
*/

#include "ChannelWhackerUI.h"

ChannelWhackerUI::ChannelWhackerUI(BRect frame, const char *name, WhackerFilt *filt)
: BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	BRect rect;
	
	// cache width and height
	width = frame.Width();
	height = frame.Height();
	
	// cache our filter
	filter = filt;
	
	// set view color
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	// add the string view
	rect.Set(5.0, 5.0, width/2.0, 20.0);
	AddChild(new BStringView(rect, "str", "Channels:"));
	
	// add the channel sliders
	// red
	rect.Set(rect.left, rect.bottom + 5.0, width - 5.0, rect.bottom + 35.0);
	redSlide = new BSlider(rect, "red", "Red", new BMessage(CW_CHANNEL_CHANGE), 0, 255);
	redSlide->SetPosition(1.0);
	AddChild(redSlide);
	// green
	rect.Set(rect.left, rect.bottom + 5.0, width - 5.0, rect.bottom + 35.0);
	greenSlide = new BSlider(rect, "green", "Green", new BMessage(CW_CHANNEL_CHANGE), 0, 255);
	greenSlide->SetPosition(1.0);
	AddChild(greenSlide);
	// blue
	rect.Set(rect.left, rect.bottom + 5.0, width - 5.0, rect.bottom + 35.0);
	blueSlide = new BSlider(rect, "blue", "Blue", new BMessage(CW_CHANNEL_CHANGE), 0, 255);
	blueSlide->SetPosition(1.0);
	AddChild(blueSlide);
	
	// we want interactivity!
	redSlide->SetSnoozeAmount(33 * 1000);
	greenSlide->SetSnoozeAmount(33 * 1000);
	blueSlide->SetSnoozeAmount(33 * 1000);
	redSlide->SetModificationMessage(new BMessage(CW_CHANNEL_CHANGE));
	greenSlide->SetModificationMessage(new BMessage(CW_CHANNEL_CHANGE));
	blueSlide->SetModificationMessage(new BMessage(CW_CHANNEL_CHANGE));
	
//	// add the three channel checkboxen
//	// red
//	rect.Set(rect.left, rect.bottom + 5.0, rect.left + 40.0, rect.bottom + 20.0);
//	AddChild(
//		redCheck = new BCheckBox(rect, "red", "Red", new BMessage(CW_CHANNEL_ADD_REMOVE)));
//	// green
//	rect.Set(rect.right + 5.0, rect.top, rect.right + 55.0, rect.bottom);
//	AddChild(
//		greenCheck = new BCheckBox(rect, "green", "Green", new BMessage(CW_CHANNEL_ADD_REMOVE)));
//	// blue
//	rect.Set(rect.right + 5.0, rect.top, rect.right + 45.0, rect.bottom);
//	AddChild(
//		blueCheck = new BCheckBox(rect, "blue", "Blue", new BMessage(CW_CHANNEL_ADD_REMOVE)));
	
	// turn all checkboxes on
//	redCheck->SetValue(1);
//	greenCheck->SetValue(1);
//	blueCheck->SetValue(1);
}

void ChannelWhackerUI::AttachedToWindow()
{
//	// aim all checkbox messages at this view
//	redCheck->SetTarget(this);
//	greenCheck->SetTarget(this);
//	blueCheck->SetTarget(this);

	Window()->SetFlags(Window()->Flags()|B_ASYNCHRONOUS_CONTROLS);
	
	// target this view
	redSlide->SetTarget(this);
	greenSlide->SetTarget(this);
	blueSlide->SetTarget(this);
}

void ChannelWhackerUI::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
//		case CW_CHANNEL_ADD_REMOVE:
//		{
//			void *source;
//			BCheckBox *check;
//			
//			if (msg->FindPointer("source", &source) == B_OK)
//			{
//				check = reinterpret_cast<BCheckBox *>(source);
//				
//				if (check == redCheck)
//				{
//					if (check->Value())
//						filter->ToggleRed(true); // on
//					else
//						filter->ToggleRed(false); // off
//				}
//				else if (check == greenCheck)
//				{
//					if (check->Value())
//						filter->ToggleGreen(true); // on
//					else
//						filter->ToggleGreen(false); // off
//				}
//				else if (check == blueCheck)
//				{
//					if (check->Value())
//						filter->ToggleBlue(true); // on
//					else
//						filter->ToggleBlue(false); // off
//				}
//			}
//			break;
//		}
		case CW_CHANNEL_CHANGE:
		{
			//printf("CW_CHANNEL_CHANGE message received\n");
			void *source;
			BSlider *slide;
			
			if (msg->FindPointer("source", &source) == B_OK)
			{
				slide = reinterpret_cast<BSlider *>(source);
				
				if (slide == redSlide)
					filter->SetRed(slide->Position());
				else if (slide == greenSlide)
					filter->SetGreen(slide->Position());
				else if (slide == blueSlide)
					filter->SetBlue(slide->Position());
			}
			break;
		}
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}

void ChannelWhackerUI::GetPreferredSize(float *w, float *h)
{
	*w = width;
	*h = height;
}