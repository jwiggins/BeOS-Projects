/*

	AboutView.cpp - A view for an Aboutbox. Nice scroller.
	John Wiggins 1998-1999

*/

#include "AboutView.h"

AboutView::AboutView(BRect frame)
	:BView(frame, "Jellikit!", B_FOLLOW_ALL, B_WILL_DRAW|B_PULSE_NEEDED)
{
	BRect bounds = Bounds(), rect;
	BFont font;
	int32 interfaceUnit = ((int32)be_plain_font->Size());
	font_height view_font_height;
	BButton *button;
	BTextView *text_view;
	rgb_color black = {0,0,0,255};
	const char *string_ptr;
	
	GetFont(&font);
	font.SetSize(32.0);
	font.GetHeight(&view_font_height);
	// the rect enclosing the J in Jellikit
	magic_rect.Set(5, (bounds.Height()/5)-view_font_height.ascent, 5+(font.StringWidth("J")), bounds.Height()/5);
	
	// the ok button
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_OK); // grab the "OK" string
	rect.Set(bounds.right - interfaceUnit - (StringWidth(string_ptr)*3), bounds.bottom - interfaceUnit*3, bounds.right - interfaceUnit, bounds.bottom - interfaceUnit);
	button = new BButton(rect, "button", string_ptr, new BMessage(B_QUIT_REQUESTED), B_FOLLOW_ALL, B_WILL_DRAW);
	button->MakeDefault(true);
	AddChild(button);
	
	// the text view
	rect.Set(interfaceUnit , 32. + interfaceUnit*2, bounds.right - interfaceUnit*3 - (StringWidth(string_ptr)*3), bounds.bottom - interfaceUnit);
	text_view = new BTextView(rect, "text view", rect, B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(text_view);
	string_ptr = NULL; // we need the width of "OK" during the text_view construction
	
	// do the text rect
	BRect text_rect = rect;
	text_rect.InsetBy(2,2);
	text_rect.OffsetTo(B_ORIGIN);
	text_view->SetTextRect(text_rect);
	text_view->MakeSelectable(false);
	text_view->MakeEditable(false);
	
	// now set it's text to something interesting
	text_view->SetText(
		"1998-1999 by John Wiggins\n\n"
		"Thanks to Dominic Giampaolo for writing bfs\n\n"
		"Thanks to Brian Tietz for writing Santa's gift bag\n\n"
		"And finally... Thanks to Primus for writing Jellikit, the song that this app was named after...\n\n");
	
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetHighColor(black);
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	// build the version string
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_VERSION); // grab the "Version" string
	version_string = (char *)malloc(strlen(string_ptr) + strlen(JELLIKIT_VERSION) + 2);
	sprintf(version_string, "%s %s", string_ptr, JELLIKIT_VERSION);
}

AboutView::~AboutView()
{
	free(version_string);
}

void AboutView::MouseDown(BPoint point)
{
	key_info kinfo;
	uint32 buttons = 0;
	get_key_info(&kinfo);
	GetMouse(&point, &buttons, false);
	
	// muhahahahahah
	if(buttons & B_SECONDARY_MOUSE_BUTTON)
	{
		if(kinfo.modifiers & B_COMMAND_KEY)
		{
			if(kinfo.modifiers & B_SHIFT_KEY)
			{
				//printf("Shift key and commaned key are pressed.\n");
				if((kinfo.key_states[(0x26)>>3] & (1 << (7 - ((0x26)%8)))))
				{
					//printf("Tab key is pressed.\n");
					if(magic_rect.Contains(point))
					{
						//printf("Mouse click was inside magic_rect.\n");
						Window()->PostMessage(DO_EASTER_EGG);
					}
				}
			}
		}
	}
}

void AboutView::Draw(BRect)
{
	BFont font;
	float cached_size;
	
	// draw the Jellikit string
	GetFont(&font);
	cached_size = font.Size();
	font.SetSize(32.0);
	SetFont(&font, B_FONT_SIZE);
	DrawString("Jellikit", BPoint(5, Bounds().Height()/5)); // big
	
	// draw the version string
	font.SetSize(cached_size);
	SetFont(&font, B_FONT_SIZE);
	MovePenBy((-PenLocation().x) + 10, cached_size);
	DrawString(version_string);
	
	// an outline around the textview
	BView *view = FindView("text view");
	BRect rect = view->Bounds();
	rect.InsetBy(-1,-1);
	StrokeRect(view->ConvertToParent(rect));
}

void AboutView::Pulse()
{
	// cheesy scroll action
	BTextView *view = static_cast<BTextView *>(FindView("text view"));
	static int32 textheight, viewheight;
	
	if(view)
	{
		textheight = (int32)view->TextHeight(0, view->TextLength());
		viewheight = view->Bounds().IntegerHeight();
		if(view->LeftTop().y < (textheight))
			view->ScrollBy(0,1);
		else
			view->ScrollBy(0, -(textheight + viewheight));
	}
}