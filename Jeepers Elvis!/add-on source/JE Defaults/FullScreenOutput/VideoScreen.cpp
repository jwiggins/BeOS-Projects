/*
	VideoScreen.cpp
	2000 John Wiggins
*/

#include "VideoScreen.h"

VideoScreen::VideoScreen(uint32 space, status_t *ret)
: BWindowScreen("FullScreen Output", space, ret), accessLock("FullScreen Access")
{
	// not connected
	connected = false;
	
	// unknown display dimensions
	disWidth = 1;
	disHeight = 1;
	
	// unknown frame size
	frmHeight = 0;
	frmWidth = 0;
	
	// and invalid scale
	xScale = 0.0f;
	yScale = 0.0f;
	
	// not valid until we connect
	frameBuffer = NULL;
	lineLength = 0;
	
	// display size change flag
	disSizeInvalid = false;
	
	// add a keyboard/mouse input BMessageFilter
	AddCommonFilter(new InputFilter);
	
	//printf("VideoScreen::VideoScreen() : IsLocked() = %s\n", (IsLocked() ? "true" : "false"));
	
	// call Hide() once before Show() to remain hidden and start the msg loop
	Hide();
	Show();
}

bool VideoScreen::QuitRequested()
{	
	return true;
}

void VideoScreen::ScreenConnected(bool active)
{
	// assign connected if we disconnected
	if (active == false)
		connected = false;
	
	// we just gained access
	if (active)
	{
		// get our display dimensions
		disWidth = FrameBufferInfo()->display_width;
		disHeight = FrameBufferInfo()->display_height;
		
		// note that the display size may be different
		disSizeInvalid = true;
		
		// and the length of a row
		lineLength = FrameBufferInfo()->bytes_per_row / 4;
		
		// get a pointer to the frame buffer
		frameBuffer = (uint32 *)(CardInfo()->frame_buffer);
		
		// zero the frame buffer
		for (int32 y = 0; y < disHeight; y++)
			for (int32 x = 0; x < disWidth; x++)
				frameBuffer[(y*lineLength)+x] = 0;
		
//		printf("VideoScreen::ScreenConnected()\n");
//		printf("disWidth = %ld, disHeight = %ld\n", disWidth, disHeight);
		
		// unlock the access lock (shouldn't be blocking any thread(s))
		accessLock.Unlock();
	}
	else // just lost access
	{
		// block on any drawing that might be taking place
		accessLock.Lock();
	}
	
	// assign connected if we connected
	if (active == true)
		connected = true;
}

void VideoScreen::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case B_KEY_DOWN:
		{
			const char *bytes;
			
			if (msg->FindString("bytes", &bytes) == B_OK)
			{
				// escape key and we aren't hidden
				if (bytes[0] == B_ESCAPE && !IsHidden())
					Hide();
			}
			
			break;
		}
		case B_KEY_UP:
		{
			break;
		}
		case B_MOUSE_MOVED:
		{
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}

void VideoScreen::Draw(BBitmap *bmp)
{
	// ensure scaling state whether we draw or not
	if (frmWidth != (int32)bmp->Bounds().Width()
		|| frmHeight != (int32)bmp->Bounds().Height()
		|| disSizeInvalid)
	{
		// assign width and height
		frmWidth = (int32)bmp->Bounds().Width();
		frmHeight = (int32)bmp->Bounds().Height();
		
		// calculate scale factors
//		xScale = disWidth / (float)frmWidth;
//		yScale = disHeight / (float)frmHeight;
		xScale = frmWidth / (float)disWidth;
		yScale = frmHeight / (float)disHeight;
		
		// no longer in question
		disSizeInvalid = false;
	}
	
	// don't draw if we aren't connected
	// or have a bad semaphore
	if (connected && accessLock.Lock())
	{
		uint32 *fBits = (uint32 *)bmp->Bits();
		int32 fRowlen = bmp->BytesPerRow() / 4;
		float xMove = 0.0f, yMove = 0.0f;
		
//		printf("VideoScreen::Draw()\n");
//		printf("frmWidth = %ld, frmHeight = %ld\n", frmWidth, frmHeight);
//		printf("disWidth = %ld, disHeight = %ld\n", disWidth, disHeight);
//		printf("xScale = %g, yScale = %g\n", xScale, yScale);
		
//		for (int32 y = 0; y < disHeight; y++)
//			for (int32 x = 0; x < disWidth; x++)
//				frameBuffer[(y*lineLength) + x] =
//				fBits[(((int32)(y*yScale))*fRowlen)+((int32)(x*xScale))];
		
		/*
			// fast scaling c.o. Hai
			xStep = sourcewidth/destWidth;
			yStep = sourceheight/destHeight;
			
			xmove = 0;
			ymove = 0; 
			
			for( int y=0; y < dstHeight; y++)
			{
				for( int x=0; x < dstWidth ; x++)
				{
					dstPixel(x,y) = srcPixel(xMove,yMove);
					xMove + xStep;
				}
				yMove += yStep;  
				xMove = 0;
			}
		*/
		// draw
		for (int32 y = 0; y < disHeight; y++)
		{
			for (int32 x = 0; x < disWidth; x++)
			{
				frameBuffer[(y*lineLength) + x] = fBits[((fRowlen*(int32)yMove) + (int32)xMove)];
				xMove += xScale;
			}
			yMove += yScale;  
			xMove = 0.0f;
		}
		
//		for (int32 y = 0; y < frmHeight; y++)
//			for (int32 x = 0; x < frmWidth; x++)
//				frameBuffer[(y*lineLength) + x] = fBits[(fRowlen*y) + x];
		
		
		// surrender access
		accessLock.Unlock();
	}
}
