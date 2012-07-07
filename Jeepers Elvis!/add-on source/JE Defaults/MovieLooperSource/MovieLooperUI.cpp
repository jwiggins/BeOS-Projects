/*
	MovieLooperUI.cpp
	2000 John Wiggins
*/

#include "MovieLooperUI.h"

/////////////////
// MovieLooperUI
/////////////////
MovieLooperUI::MovieLooperUI(BRect frame, const char *name,
							VideoReader *vid, BBitmap **bmp, BLocker *lock)
: BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	BRect rect;
	BMessage *msg;
	
	// the position indicator
	rect.Set(5.0, 5.0, frame.Width()/2.0, 19.0);
	vidPosition = new PositionIndicator(rect, "vidpos");
	AddChild(vidPosition);
	
	// the ClipListView
	// inside of a BScrollView
	rect.Set(5.0, rect.bottom+10.0, rect.right-B_V_SCROLL_BAR_WIDTH, frame.Height()-5.0);
	listView = new ClipListView(rect, "cliplist", (BView *)this);
	AddChild(new BScrollView("scrollclips", listView, B_FOLLOW_LEFT|B_FOLLOW_TOP, 0, false, true));
	// set the selection message for the list view
	listView->SetSelectionMessage(new BMessage(ML_LOAD_VIDEO_CLIP));
	
	// the radio button group
	// forwards
	rect.Set((frame.Width()/2.0)+5.0, 5.0, frame.Width()-5.0, 14.0);
	msg = new BMessage(ML_SET_PLAY_DIRECTION);
	msg->AddInt32("dir", DIR_FORWARDS);
	AddChild(radio1 = new BRadioButton(rect, "radio1", "Forwards", msg));
	// backwards
	rect.Set(rect.left, rect.bottom+5.0, rect.right, rect.bottom+14.0);
	msg = new BMessage(ML_SET_PLAY_DIRECTION);
	msg->AddInt32("dir", DIR_BACKWARDS);
	AddChild(radio2 = new BRadioButton(rect, "radio2", "Backwards", msg));
	// back and forth
	rect.Set(rect.left, rect.bottom+5.0, rect.right, rect.bottom+14.0);
	msg = new BMessage(ML_SET_PLAY_DIRECTION);
	msg->AddInt32("dir", DIR_BACK_AND_FORTH);
	AddChild(radio3 = new BRadioButton(rect, "radio3", "Back & Forth", msg));
	// set the "Forwards" radio button
	((BRadioButton *)FindView("radio1"))->SetValue(1);
	
	// the BStringViews in charge of Start/End text representation
	// Start
	rect.Set(rect.left, rect.bottom+15.0, rect.right, rect.bottom+29.0);
	AddChild(startStr = new BStringView(rect, "startstr", "Start:"));
	// End
	rect.Set(rect.left, rect.bottom+5.0, rect.right, rect.bottom+19.0);
	AddChild(endStr = new BStringView(rect, "endstr", "End:"));
	
	// the preferred width and height
	prefWidth = frame.Width();
	prefHeight = frame.Height();
	
	// current video play direction
	curDirection = DIR_FORWARDS;
	
	// the VideoReader
	videoReader = vid;
	
	// the bitmap for the video frame
	frameMap = bmp;
	
	// the mutex protecting the video frame bitmap
	frameMutex = lock;
	
	// set the view color
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

MovieLooperUI::~MovieLooperUI()
{
}

void MovieLooperUI::GetPreferredSize(float *w, float *h)
{
	*w = prefWidth;
	*h = prefHeight;
}

void MovieLooperUI::AttachedToWindow()
{
	//printf("MovieLooperUI::AttachedToWindow()\n");
	// set the target for the ClipListView
	listView->SetTarget(this);
	// set the targets for all the radio buttons
	radio1->SetTarget(this);
	radio2->SetTarget(this);
	radio3->SetTarget(this);
	// tell the videoReader where we are
	videoReader->SetUIMessenger(this);
}

void MovieLooperUI::MessageReceived(BMessage *msg)
{
	//printf("MovieLooperUI::MessageReceived()\n");
	//printf("msg->IsSourceRemote() = %d\n", msg->IsSourceRemote());
	//msg->PrintToStream();
	// check for dropped messages
	if (msg->WasDropped())
	{
		entry_ref ref;
		int32 index = 0;
		
		// loop through the refs in the message
		while (msg->FindRef("refs", index, &ref) == B_OK)
		{
			// if it's a proper video file
			if (videoReader->SniffFile(&ref) == B_OK)
			{
				// and it loads
				if (videoReader->LoadFile(&ref) == B_OK)
				{
					// then add it to the listview
					listView->AddItem(new BStringItem(ref.name));
				}
			}
			index++;
		}
	}
	else
	{
		switch(msg->what)
		{
			case ML_SET_START_POINT:
			{
				int64 where;
				
				if (msg->FindInt64("where", &where) == B_OK)
				{
					// tell videoReader
					if (videoReader->Lock())
					{
						videoReader->SetClipStart(where);
						videoReader->Unlock();
					}
					
					// tell startStr
					BString str("Start: ");
					str << where;
					startStr->SetText(str.String());
				}
				
				break;
			}
			case ML_SET_END_POINT:
			{
				int64 where;
				
				if (msg->FindInt64("where", &where) == B_OK)
				{
					// tell videoReader
					if (videoReader->Lock())
					{
						videoReader->SetClipEnd(where);
						videoReader->Unlock();
					}
					
					// tell endStr
					BString str("End: ");
					str << where;
					endStr->SetText(str.String());
				}
				
				break;
			}
			case ML_SET_INDICATOR_POSITION:
			{
				int64 where;
				
				if (msg->FindInt64("where", &where) == B_OK)
				{
					vidPosition->SetPosition(where);
				}
				
				break;
			}
			case ML_SET_PLAY_DIRECTION:
			{
				clip_direction dir;
				
				//printf("MovieLooperUI::MessageReceived(ML_SET_PLAY_DIRECTION)\n");
				if (msg->FindInt32("dir", (int32 *)&dir) == B_OK)
				{
					// set curDirection
					curDirection = dir;
					// tell videoReader about it
					if (videoReader->Lock())
					{
						videoReader->SetClipDirection(dir);
						videoReader->Unlock();
					}
				}
				 
				break;
			}
			case ML_LOAD_VIDEO_CLIP:
			{
				int32 index;
				
				//printf("MovieLooperUI::MessageReceived(ML_LOAD_VIDEO_CLIP)\n");
				index = listView->CurrentSelection();
				if (index >= 0 && frameMutex->Lock() && videoReader->Lock())
				{
					// load the clip
					videoReader->LoadClipForPlaying(index, frameMap);
					videoReader->Unlock();
					// unlock the frame data
					frameMutex->Unlock();
					
					// once LoadClipForPlaying() returns, the clip is loaded
//					printf("Immediately after load:\n");
//					printf("clip->Length = %Ld frames, %Ld usecs\n",
//						videoReader->ClipLengthFrames(), videoReader->ClipLengthTime());
//					printf("clip->Start = frame %Ld\n", videoReader->ClipStart());
//					printf("clip->End = frame %Ld\n", videoReader->ClipEnd());
//					printf("clip->Position = frame %Ld\n", videoReader->CurrentFrameNumber());
					// reset vidPosition
					vidPosition->SetLength(videoReader->ClipLengthFrames(),
									 videoReader->ClipLengthTime());
					vidPosition->SetStartWithRedraw(videoReader->ClipStart());
					vidPosition->SetEndWithRedraw(videoReader->ClipEnd());
					vidPosition->SetPosition(videoReader->CurrentFrameNumber());
					// force redraw of vidPosition
					vidPosition->Invalidate(vidPosition->Bounds());
					// note the direction
					curDirection = videoReader->ClipDirection();
					// adjust the radio buttons
					switch(curDirection)
					{
						case DIR_FORWARDS:
						{
							radio1->SetValue(1);
							break;
						}
						case DIR_BACKWARDS:
						{
							radio2->SetValue(1);
							break;
						}
						case DIR_BACK_AND_FORTH:
						{
							radio3->SetValue(1);
							break;
						}
						default:
							break;
					}
					
//					printf("After GUI foo:\n");
//					printf("clip->Length = %Ld frames, %Ld usecs\n",
//						videoReader->ClipLengthFrames(), videoReader->ClipLengthTime());
//					printf("clip->Start = frame %Ld\n", videoReader->ClipStart());
//					printf("clip->End = frame %Ld\n", videoReader->ClipEnd());
//					printf("clip->Position = frame %Ld\n", videoReader->CurrentFrameNumber());
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
}
////////////////
// ClipListView
////////////////

MovieLooperUI::ClipListView::ClipListView(BRect frame, const char *name, BView *parent)
: BListView(frame, name)
{
	highlightColor.red = 255;
	highlightColor.green = 0;
	highlightColor.blue = 0;
	highlightColor.alpha = 255;
	
	drawHighlight = false;
	parentView = parent;
}

MovieLooperUI::ClipListView::~ClipListView()
{
	// empty out the list
	BListItem *item = NULL;
	while ((item = RemoveItem(0L)) != NULL)
		delete item;
}

void MovieLooperUI::ClipListView::Draw(BRect update)
{	
	// add a highlight if needed
	if (drawHighlight)
	{
		// inherited version first
		BListView::Draw(update);
		
		// then the highlight
		rgb_color highColor = HighColor();
		SetHighColor(highlightColor);
		StrokeRect(Bounds());
		SetHighColor(highColor);
	}
	else
	{
		// the "non-highlight" (takes care of erasing)
		StrokeRect(Bounds(), B_SOLID_LOW);
		// inherited version last
		BListView::Draw(update);
	}
}

void MovieLooperUI::ClipListView::MessageReceived(BMessage *msg)
{
	// pass on dropped messages (with refs inside) to the parent view
	if (msg->WasDropped())
	{
		type_code type;
		if (msg->GetInfo("refs", &type) == B_OK)
		{
			parentView->MessageReceived(msg);
		}
	}
	else
	{
		switch(msg->what)
		{
			case ML_LOAD_VIDEO_CLIP:
			{
				parentView->MessageReceived(msg);
				break;
			}
			default:
			{
				BListView::MessageReceived(msg);
				break;
			}
		}
	}
}

void MovieLooperUI::ClipListView::MouseUp(BPoint pt)
{
	BListView::MouseUp(pt);
	if (drawHighlight)
	{
		drawHighlight = false;
		Draw(Bounds());
	}
}

void MovieLooperUI::ClipListView::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
	BListView::MouseMoved(pt, code, msg);
	
	type_code type;
	if (msg != NULL && msg->GetInfo("refs", &type) == B_OK)
	{
		// this looks suspiciously like a drag message from the Tracker
		if (code == B_ENTERED_VIEW)
		{
			drawHighlight = true;
			Draw(Bounds());
		}
		else if (code == B_EXITED_VIEW)
		{
			drawHighlight = false;
			Draw(Bounds());
		}
	}
}

/////////////////////
// PositionIndicator
/////////////////////
MovieLooperUI::PositionIndicator::PositionIndicator(BRect frame, const char *name)
: BView(frame, name, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW|B_NAVIGABLE)
{
	navigationColor = ui_color(B_KEYBOARD_NAVIGATION_COLOR);
	
	knobColor.red = 192;
	knobColor.green = 0;
	knobColor.blue = 0;
	knobColor.alpha = 255;
	
	clipColor.red = 171;
	clipColor.green = 221;
	clipColor.blue = 161;
	clipColor.alpha = 255;
	
	unClipColor.red = 128;
	unClipColor.green = 128;
	unClipColor.blue = 128;
	unClipColor.alpha = 255;
	
	startCoord = 3.0;
	playCoord = 1.0;
	endCoord = frame.Width()-3.0;
	startPos = 0;
	playPos = 0;
	endPos = 0;
	frameCount = 0;
	duration = 0;
	startMoving = false;
	endMoving = false;
	
	SetViewColor(B_TRANSPARENT_COLOR);
}

void MovieLooperUI::PositionIndicator::Draw(BRect update)
{
	BRect rect = Bounds();
	
	// draw the focus indicator
	if (IsFocus())
	{
		SetHighColor(navigationColor);
		StrokeRect(rect);
	}
	else // or not
	{
		// how bout a nice bevel?
		SetHighColor(255,255,255,255);
		StrokeLine(rect.RightTop(), rect.RightBottom());
		StrokeLine(rect.LeftBottom(), rect.RightBottom());
		SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
		StrokeLine(rect.LeftTop(), rect.RightTop());
		StrokeLine(rect.LeftTop(), rect.LeftBottom());
	}
	
	// the non-focus-indicating part of the view
	rect.InsetBy(1.0,1.0);
	
	// everything but the knobs
	if (startCoord == 3.0 && endCoord == rect.right-2.0)
	{
		// green
		SetHighColor(clipColor);
		// don't draw where the knobs draw
		rect.left += 5.0;
		rect.right -= 5.0;
		// draw
		FillRect(rect & update);
	}
	else
	{
		BRect left(rect.left, rect.top, startCoord-3.0, rect.bottom),
				right(endCoord+3.0, rect.top, rect.right, rect.bottom),
				middle(startCoord+3.0, rect.top, endCoord-3.0, rect.bottom);
		
		SetHighColor(clipColor);
		FillRect(middle & update);
		SetHighColor(unClipColor);
		FillRect(left & update);
		FillRect(right & update);
	}
	
	// the position indicator
	if (update.left < playCoord && update.right > playCoord)
	{
		if (playCoord >= (startCoord+3.0) && playCoord <= (endCoord-3.0))
		{
			SetHighColor(0,0,0,255);
			StrokeLine(BPoint(playCoord, 1.0), BPoint(playCoord, Bounds().bottom-1.0));
		}
	}
	// and the knobs
	rect.Set(startCoord-2.0, 1.0, startCoord+2.0, Bounds().bottom-1.0);
	if (update.Intersects(rect))
		DrawKnob(startCoord);
	
	rect.Set(endCoord-2.0, 1.0, endCoord+2.0, Bounds().bottom-1.0);
	if (update.Intersects(rect))
		DrawKnob(endCoord);
}

void MovieLooperUI::PositionIndicator::MouseDown(BPoint pt)
{
	if (pt.x <= startCoord + 2.0 && pt.x >= startCoord - 2.0)
	{
		// they clicked on the start position knob
		SetMouseEventMask(B_POINTER_EVENTS);
		startMoving = true;
	}
	else if (pt.x <= endCoord + 2.0 && pt.x >= endCoord - 2.0)
	{
		// they clicked on the end position knob
		SetMouseEventMask(B_POINTER_EVENTS);
		endMoving = true;
	}
}

void MovieLooperUI::PositionIndicator::MouseUp(BPoint pt)
{
	startMoving = false;
	endMoving = false;
}

void MovieLooperUI::PositionIndicator::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
	if (startMoving)
	{
		// if there is a change in x
		// and it's not too small
		// and it's not past endCoord
		if (floor(pt.x) != floor(startCoord)
			&& pt.x >= 3.0
			&& pt.x <= (endCoord-5.0))
		{
			SetStart((int64)floor((pt.x/(Bounds().Width()-6.0))*frameCount));
			float oldCoord = startCoord;
			startCoord = pt.x;
			if (oldCoord > startCoord)
				Draw(BRect(1.0,1.0,oldCoord+4.0,Bounds().bottom-1.0));
			else
				Draw(BRect(1.0,1.0,startCoord+4.0,Bounds().bottom-1.0));
		}
	}
	else if (endMoving)
	{
		// if there is a change in x
		// and it's not too big
		// and it's not past startCoord
		if (floor(pt.x) != floor(endCoord)
			&& pt.x <= Bounds().right-3.0
			&& pt.x >= (startCoord+5.0))
		{
			SetEnd((int64)floor((pt.x/(Bounds().Width()-6.0))*frameCount));
			float oldCoord = endCoord;
			endCoord = pt.x;
			if (oldCoord < endCoord)
				Draw(BRect(oldCoord-4.0,1.0,Bounds().right-1.0,Bounds().bottom-1.0));
			else
				Draw(BRect(endCoord-4.0,1.0,Bounds().right-1.0,Bounds().bottom-1.0));
			
		}
	}
}

void MovieLooperUI::PositionIndicator::KeyDown(const char *bytes, int32 numBytes)
{
	if (bytes[0] == B_RIGHT_ARROW)
	{
		if (modifiers() & B_SHIFT_KEY)
			StepEndForward();
		else
			StepStartForward();
	}
	else if (bytes[0] == B_LEFT_ARROW)
	{
		if (modifiers() & B_SHIFT_KEY)
			StepEndBackward();
		else
			StepStartBackward();
	}
	else
		BView::KeyDown(bytes, numBytes);
}

void MovieLooperUI::PositionIndicator::KeyUp(const char *bytes, int32 numBytes)
{
	BView::KeyUp(bytes, numBytes);
}

void MovieLooperUI::PositionIndicator::MakeFocus(bool focusState)
{
	BView::MakeFocus(focusState);
	Invalidate(Bounds());
}

void MovieLooperUI::PositionIndicator::SetLength(int64 frames, bigtime_t dur)
{
	frameCount = frames;
	duration = dur;
	
	ResetPoints();
}

void MovieLooperUI::PositionIndicator::SetStart(int64 frame)
{
	if (frame >= 0 && frame <= frameCount && frame <= endPos)
		startPos = frame;
	
	// tell your parent
	BMessage msg(ML_SET_START_POINT);
	msg.AddInt64("where", startPos);
	Parent()->MessageReceived(&msg);
}


void MovieLooperUI::PositionIndicator::SetStartWithRedraw(int64 frame)
{
	if (frame >= 0 && frame <= frameCount && frame <= endPos)
		startPos = frame;
	
	// tell your parent
	BMessage msg(ML_SET_START_POINT);
	msg.AddInt64("where", startPos);
	Parent()->MessageReceived(&msg);
	
	// now set the knob coordinate
	if (startPos == 0)
		startCoord = 3.0;
	else
	{
		startCoord = (3.0 + ((startPos/(float)frameCount)*(Bounds().Width()-6.0)));
		// don't go off the end
		if (startCoord > (Bounds().right-3.0))
			startCoord = Bounds().right-3.0;
		// don't go past endCoord
		if ((endCoord - startCoord) < 5.0)
			startCoord = endCoord - 5.0;
	}
	// draw
	Draw(BRect(1.0,1.0,startCoord+3.0,Bounds().bottom-1.0));
}

void MovieLooperUI::PositionIndicator::SetPosition(int64 frame)
{
	float oldCoord = playCoord;
	
	// assume looper is Lock()ed
	if (frame >= 0 && frame <= frameCount)
		playPos = frame;
	
	// calculate the bar pos
	if (playPos == 0)
		playCoord = 1.0;
	else
		playCoord = floor(1.0+((playPos/(float)frameCount)*(Bounds().Width()-4.0)));
	// update the view
	if (oldCoord < playCoord)
		Draw(BRect(oldCoord-1.0, 1.0, playCoord+1.0, Bounds().bottom-1.0));
	else
		Draw(BRect(playCoord-1.0, 1.0, oldCoord+1.0, Bounds().bottom-1.0));
}

void MovieLooperUI::PositionIndicator::SetEnd(int64 frame)
{
	if (frame >= 0 && frame <= frameCount && frame >= startPos)
		endPos = frame;
	
	// tell your parent
	BMessage msg(ML_SET_END_POINT);
	msg.AddInt64("where", endPos);
	Parent()->MessageReceived(&msg);
}

void MovieLooperUI::PositionIndicator::SetEndWithRedraw(int64 frame)
{
	if (frame >= 0 && frame <= frameCount && frame >= startPos)
		endPos = frame;
	
	// tell your parent
	BMessage msg(ML_SET_END_POINT);
	msg.AddInt64("where", endPos);
	Parent()->MessageReceived(&msg);
	
	// now set the knob coordinate
	if (endPos == 0)
		endCoord = 2.0;
	else
	{
		endCoord = (3.0 + ((endPos/(float)frameCount)*(Bounds().Width()-6.0)));
		// don't go off the end
		if (endCoord > (Bounds().right-3.0))
			endCoord = Bounds().right-3.0;
		// don't go past startCoord
		if ((endCoord - startCoord) < 5.0)
			endCoord = startCoord + 5.0;
	}
	// draw
	Draw(BRect(endCoord-3.0,1.0,Bounds().right-1.0,Bounds().bottom-1.0));
}

void MovieLooperUI::PositionIndicator::StepStartForward()
{
	if (startPos < endPos)
		SetStartWithRedraw(startPos+1);
}

void MovieLooperUI::PositionIndicator::StepStartBackward()
{
	if (startPos > 0)
		SetStartWithRedraw(startPos-1);
}

void MovieLooperUI::PositionIndicator::StepEndForward()
{
	if (endPos < frameCount)
		SetEndWithRedraw(endPos+1);
}

void MovieLooperUI::PositionIndicator::StepEndBackward()
{
	if (endPos > startPos)
		SetEndWithRedraw(endPos-1);
}

void MovieLooperUI::PositionIndicator::ResetPoints()
{
	startPos = 0;
	playPos = 0;
	endPos = frameCount;
	
//	startCoord = 3.0;
//	playCoord = 1.0;
//	endCoord = Bounds().right-3.0;
	
	// redraw
	//Draw(Bounds().InsetByCopy(1.0, 1.0));
}

void MovieLooperUI::PositionIndicator::DrawKnob(float x)
{
	BRect rect(x-2.0, 1.0, x+2.0, Bounds().bottom-1.0);
	rgb_color highColor = HighColor();
	
	SetHighColor(knobColor);
	FillRect(rect);
	SetHighColor(highColor);
}