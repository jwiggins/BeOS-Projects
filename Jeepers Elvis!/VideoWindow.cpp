/*
	VideoWindow.cpp
	2000 John Wiggins
*/

#include "VideoWindow.h"

///////////////////////
// VideoWindow Class //
///////////////////////
VideoWindow::VideoWindow(BRect frame, const char *name, bool small)
: BWindow(frame, name, B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_ZOOMABLE|B_NOT_RESIZABLE|B_NOT_CLOSABLE)
{
	// register the window
	BMessage registry(JE_WINDOW_REGISTRY_ADD);
	be_app->PostMessage(&registry);
	
	// construct the child video view
	vidView = new VideoView(Bounds(), small);
	AddChild(vidView);
	
	// show yourself
	Show();
}

void VideoWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case JE_DRAW_BITMAP:
		{
			//printf("VideoWindow::MessageReceived(JE_DRAW_BITMAP)\n");
			void *bitmap;
			
			if (msg->FindPointer("bitmap", &bitmap) == B_OK)
			{
				vidView->UpdateFrame(reinterpret_cast<BBitmap *>(bitmap));
			}
			
			break;
		}
		case JE_APP_IS_QUITTING:
		{
			// tell yourself to quit
			PostMessage(B_QUIT_REQUESTED);
			
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}

bool VideoWindow::QuitRequested()
{
	//printf("%s\n", Title());
	//Frame().PrintToStream();
	//Bounds().PrintToStream();
	
	// un-register the window
	BMessage registry(JE_WINDOW_REGISTRY_SUB);
	be_app->PostMessage(&registry);
	return true;
}

/////////////////////
// VideoView Class //
/////////////////////
VideoWindow::VideoView::VideoView(BRect frame, bool small)
: BView(frame, "VideoView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	// set the size of the video frame
	if (small)
		vidBounds.Set(0.0,0.0,159.0,119.0); // 160x120
	else
		vidBounds.Set(0.0,0.0,319.0,239.0); // 320x240
	
	// construct the bitmap for the video frame
	vidFrame = new BBitmap(vidBounds, B_RGB32, true);
	if (vidFrame->InitCheck() != B_OK)
	{
		delete vidFrame;
		vidFrame = NULL;
	}
	else
		vidFrame->AddChild(new BView(vidFrame->Bounds(), "draw", B_FOLLOW_NONE, 0));
	
	// then offset the video rect
	vidBounds.OffsetBy(5.0,5.0);
	
	// setup nonVidBounds
	nonVidBounds.Set(Bounds());
	nonVidBounds.Exclude(vidBounds);
	
	// set our view color
	SetViewColor(B_TRANSPARENT_COLOR);
	SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	// are we small?
	isSmall = small;
	
	// init the bitmap
	DrawColorBars();
}

VideoWindow::VideoView::~VideoView()
{
	if (vidFrame != NULL)
		delete vidFrame;
}

void VideoWindow::VideoView::Draw(BRect update)
{
	if (vidBounds.Contains(update))
	{
		if (vidFrame != NULL)
		{
			// intersection of the bitmap's rect with the update rect
			BRect src(vidBounds & update), dst(src);
			
			// back to the bitmaps coord system
			src.OffsetBy(-5.0,-5.0);
			
			// and draw
			DrawBitmap(vidFrame, src, dst);
		}
	}
	else
	{
		// we have other parts of the view to worry about
		
		// but first, the video frame
		if (vidFrame != NULL)
		{
			// intersection of the bitmap's rect with the update rect
			BRect src(vidBounds & update), dst(src);
			
			// back to the bitmaps coord system
			src.OffsetBy(-5.0,-5.0);
			
			// and draw
			DrawBitmapAsync(vidFrame, src, dst);
		}
		
		// then the border
		BRect rect;
		
		for (int32 i=0; i < nonVidBounds.CountRects(); i++)
		{
			rect = nonVidBounds.RectAt(i);
			
			if (rect.Intersects(update))
				FillRect(rect & update);
		}
		
		// and Sync
		Sync();
	}
}

void VideoWindow::VideoView::UpdateFrame(BBitmap *frame)
{
	if (vidFrame != NULL)
	{
		if (frame != NULL)
		{
			// lock
			if (vidFrame->Lock())
			{
				// draw, scale to fit
				vidFrame->ChildAt(0)->DrawBitmapAsync(frame, frame->Bounds(), vidFrame->Bounds());
				// sync
				vidFrame->ChildAt(0)->Sync();
				// unlock
				vidFrame->Unlock();
			}
		}
		else
		{
			// NULL frame!
			// draw the quasi-NTSC color bars
			DrawColorBars();
		}
		
		// then put it on the screen
		Invalidate(vidBounds);
	}
}

void VideoWindow::VideoView::DrawColorBars()
{
	if (vidFrame != NULL)
	{
		// fill vidFrame with color bars
		uint32 red, green, blue, white, black, grey, cyan, magenta, yellow;
		uint32 *bits = (uint32 *)vidFrame->Bits();
		int32 rowlen = vidFrame->BytesPerRow() / 4, x, y;
		
		// GraphicsDefs.h sez - B_RGB32 is B[7:0]  G[7:0]  R[7:0]  -[7:0]
//		grey = 0xcccccc00; // 204,204,204
//		yellow = 0x00ffff00; // 255,255,0
//		cyan = 0xffff0000; // 0,255,255
//		green = 0x00ff0000; // 0,255,0
//		magenta = 0xff00ff00; // 255,0,255
//		red = 0x0000ff00; // 255,0,0
//		blue = 0xff000000; // 0,0,255
//		white = 0xffffff00; // 255,255,255
//		black = 0x00000000; // 0,0,0
		
		// B_RGB32 appears to be  -[7:0]  R[7:0]  G[7:0]  B[7:0]
		// Ah ha! The Be Book has the real truth.
		grey = 0x00cccccc; // 204,204,204
		yellow = 0x00ffff00; // 255,255,0
		cyan = 0x0000ffff; // 0,255,255
		green = 0x0000ff00; // 0,255,0
		magenta = 0x00ff00ff; // 255,0,255
		red = 0x00ff0000; // 255,0,0
		blue = 0x000000ff; // 0,0,255
		white = 0x00ffffff; // 255,255,255
		black = 0x00000000; // 0,0,0
		
		// the bars.
		// grey		yellow	cyan	green	magenta	red		blue	(70 %)
		// blue		black	magenta	black	cyan	black	grey	(10 %)
		// black	black	black	black	black	black	black	(20 %)
		
		if (isSmall)
		{
			// each bar 23 pixels wide @ 160x120. right bar is 22 pixels
			// now to draw the bars
			// top 70 %
			for (y=0; y < 84; y++)
			{
				for (x = 0; x < rowlen; x++)
				{
					if (x < 23)
						bits[(y*rowlen) + x] = grey;
					else if (x < 46)
						bits[(y*rowlen) + x] = yellow;
					else if (x < 69)
						bits[(y*rowlen) + x] = cyan;
					else if (x < 92)
						bits[(y*rowlen) + x] = green;
					else if (x < 115)
						bits[(y*rowlen) + x] = magenta;
					else if (x < 138)
						bits[(y*rowlen) + x] = red;
					else
						bits[(y*rowlen) + x] = blue;
				}
			}
			
			// next 10 %
			for (y = 84; y < 96; y++)
			{
				for (x = 0; x < rowlen; x++)
				{
					if (x < 23)
						bits[(y*rowlen) + x] = blue;
					else if (x < 46)
						bits[(y*rowlen) + x] = black;
					else if (x < 69)
						bits[(y*rowlen) + x] = magenta;
					else if (x < 92)
						bits[(y*rowlen) + x] = black;
					else if (x < 115)
						bits[(y*rowlen) + x] = cyan;
					else if (x < 138)
						bits[(y*rowlen) + x] = black;
					else
						bits[(y*rowlen) + x] = grey;
				}
			}
			
			// the last 20 %
			for (y = 96; y < 120; y++)
				for (x = 0; x < rowlen; x++)
					bits[(y*rowlen) + x] = black;
		}
		else // !isSmall
		{
			// each bar 46 pixels wide @ 320x240. right bar is 44 pixels
			// now to draw the bars
			// top 70 %
			for (y=0; y < 168; y++)
			{
				for (x = 0; x < rowlen; x++)
				{
					if (x < 46)
						bits[(y*rowlen) + x] = grey;
					else if (x < 92)
						bits[(y*rowlen) + x] = yellow;
					else if (x < 138)
						bits[(y*rowlen) + x] = cyan;
					else if (x < 184)
						bits[(y*rowlen) + x] = green;
					else if (x < 230)
						bits[(y*rowlen) + x] = magenta;
					else if (x < 274)
						bits[(y*rowlen) + x] = red;
					else
						bits[(y*rowlen) + x] = blue;
				}
			}
			
			// next 10 %
			for (y = 168; y < 192; y++)
			{
				for (x = 0; x < rowlen; x++)
				{
					if (x < 46)
						bits[(y*rowlen) + x] = blue;
					else if (x < 92)
						bits[(y*rowlen) + x] = black;
					else if (x < 138)
						bits[(y*rowlen) + x] = magenta;
					else if (x < 184)
						bits[(y*rowlen) + x] = black;
					else if (x < 230)
						bits[(y*rowlen) + x] = cyan;
					else if (x < 274)
						bits[(y*rowlen) + x] = black;
					else
						bits[(y*rowlen) + x] = grey;
				}
			}
			
			// the last 20 %
			for (y = 192; y < 240; y++)
				for (x = 0; x < rowlen; x++)
					bits[(y*rowlen) + x] = black;
		}
	}
}
