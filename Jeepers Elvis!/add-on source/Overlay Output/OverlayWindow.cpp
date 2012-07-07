/*
	OverlayWindow.cpp
	2002 John Wiggins
*/

#include "OverlayWindow.h"


OverlayView::OverlayView(BRect frame)
: BView(frame, "overlay view", B_FOLLOW_ALL, B_WILL_DRAW) , bmpBounds()
{
	bitmap = NULL;
	usingOverlay = false;
	cSpace = B_RGB32;
}

OverlayView::~OverlayView()
{
	if (bitmap)
	{
//		if (usingOverlay)
//			ClearViewOverlay();
		delete bitmap;
	}
}

void OverlayView::Draw(BRect update)
{
	if (bitmap && !usingOverlay)
	{
		DrawBitmap(bitmap, Bounds());
	}
}

void OverlayView::KeyDown(const char *bytes, int32 numBytes)
{
	// escape key and we aren't hidden
	if (bytes[0] == B_ESCAPE && !Window()->IsHidden())
		Window()->PostMessage(OO_VISIBILITY_TOGGLE);
}

void OverlayView::SetBitmap(BBitmap *bmp)
{
//	printf("OverlayView::SetBitmap()\n");
	// lock the window first in case we are being called from outside of our thread
	// !!! I don't _think_ this is a possible deadlock, but I might be wrong
	if (Window()->Lock())
	{
		// out with the old
		if (bitmap)
		{
			if (usingOverlay)
				ClearViewOverlay();
			delete bitmap;
		}
		
		// make new bitmap with the same properties as the arg
		bitmap = new BBitmap(bmp->Bounds(), B_BITMAP_WILL_OVERLAY|B_BITMAP_RESERVE_OVERLAY_CHANNEL, bmp->ColorSpace());
		usingOverlay = true;
		if (bitmap->InitCheck() != B_OK)
		{
			delete bitmap;
			bitmap = new BBitmap(bmp->Bounds(), B_BITMAP_WILL_OVERLAY|B_BITMAP_RESERVE_OVERLAY_CHANNEL, B_RGB16);
			
			if (bitmap->InitCheck() != B_OK)
			{
				delete bitmap;
				bitmap = new BBitmap(bmp->Bounds(), bmp->ColorSpace());
				usingOverlay = false;
			}
		}
		
//		printf("OverlayView::SetBitmap() :\n overlay is %s\n", usingOverlay ? "on" : "off");
//		printf("bitmap colorspace is ");
//		switch(bitmap->ColorSpace())
//		{
//			case B_RGB32:
//			{
//				printf("B_RGB32\n");
//				break;
//			}
//			case B_RGB16:
//			{
//				printf("B_RGB16\n");
//				break;
//			}
//			default:
//			{
//				printf("unknown: %08x\n", bmp->ColorSpace());
//				break;
//			}
//		}
		
		// cache the bitmap's bounds rectangle
		bmpBounds = bitmap->Bounds();
		
		// fill new bitmap with data
		bitmap->LockBits();
		memcpy(bitmap->Bits(), bmp->Bits(), bitmap->BitsLength());
		bitmap->UnlockBits();
		
		// make the bitmap an overlay
		if (usingOverlay)
		{
//			overlay_restrictions r;
//			bitmap->GetOverlayRestrictions(&r);
//			printf("Overlay limits:\n");
//			printf("  Src horiz alignment  : %08x\n",r.source.horizontal_alignment);
//			printf("  Src vert alignment   : %08x\n",r.source.vertical_alignment);
//			printf("  Src width alignment  : %08x\n",r.source.width_alignment);
//			printf("  Src height alignment : %08x\n",r.source.height_alignment);
//			printf("  Src min/max          : (%d,%d)/(%d,%d)\n",r.source.min_width,r.source.min_height,
//																r.source.max_width,r.source.max_height);
//			printf("  Dst horiz alignment  : %08x\n",r.destination.horizontal_alignment);
//			printf("  Dst vert alignment   : %08x\n",r.destination.vertical_alignment);
//			printf("  Dst width alignment  : %08x\n",r.destination.width_alignment);
//			printf("  Dst height alignment : %08x\n",r.destination.height_alignment);
//			printf("  Dst min/max          : (%d,%d)/(%d,%d)\n",r.destination.min_width,r.destination.min_height,
//																r.destination.max_width,r.destination.max_height);
//			printf("  Min/max scaling      : (%f,%f)/(%f,%f)\n",r.min_width_scale,r.min_height_scale,
//															r.max_width_scale,r.max_height_scale);

			rgb_color key;
			// make the bitmap an overlay
			SetViewOverlay(bitmap,bitmap->Bounds(),Bounds(),&key,B_FOLLOW_ALL,
				B_OVERLAY_FILTER_HORIZONTAL|B_OVERLAY_FILTER_VERTICAL);
			// set the view color to the overlay's key color
			SetViewColor(key);
			
			// cache the colorspace of our overlay
			cSpace = bitmap->ColorSpace();
		}
		else
		{
			SetViewColor(B_TRANSPARENT_32_BIT);
		}
		
		// unlock the window
		Window()->Unlock();
	}
}

void OverlayView::UpdateBits(BBitmap *bmp)
{
	//printf("OverlayView::UpdateBits()\n");
	// make sure the bitmap's dimensions haven't changed
	// colorspace should be constant as per the JE! design
	if(!bitmap || bmp->Bounds() != bmpBounds)
	{
		SetBitmap(bmp);
	}
	else
	{
		// fill our bitmap with data
		if (usingOverlay)
		{ 
			bitmap->LockBits();
			
			switch(cSpace)
			{
				case B_RGB32:
				{
					// just a simple memcpy
					memcpy(bitmap->Bits(), bmp->Bits(), bitmap->BitsLength());
					break;
				}
				case B_RGB16:
				{
					// fucking voodoo, man
					// (only because headers LIE)
					uint32 *sbits = (uint32 *)bmp->Bits();
					uint16 *dbits = (uint16 *)bitmap->Bits();
					int32 srowlen = bmp->BytesPerRow()/4,
						drowlen = bitmap->BytesPerRow()/2,
						dlines = bitmap->BitsLength()/bitmap->BytesPerRow();
					
					
					for (int y=0; y < dlines; y++)
					{
						for (int x=0; x < drowlen; x++)
						{
							uint32 pixel;
							uint16 r,g,b, val;
							
							// source pixel
							pixel = sbits[srowlen*y + x];
							
							r = (uint16)(pixel>>16 & 0xff);
							g = (uint16)(pixel>>8 & 0xff);
							b = (uint16)(pixel & 0xff);
							
							// smoosh back together					
							// All praise BGA! (StampTV creator)
							/*R[4:0],G[5:3]  G[2:0],B[4:0]*/
							val = ((r << 8) & (0x1f << 11))
								+ ((g << 3) & (0x3f << 5))
								+ ((b >> 3) & 0x1f);
							
							// dest pixel
							dbits[drowlen*y + x] = val;
						}
					}
					
					break;
				}
				default:
				{
					// we shouldn't get here
					usingOverlay = false;
					break;
				}
			}
			bitmap->UnlockBits();
		}
		else
		{
			bitmap->LockBits();
			memcpy(bitmap->Bits(), bmp->Bits(), bitmap->BitsLength());
			bitmap->UnlockBits();
		}
	}
	
	// refresh the display
	if (!usingOverlay)
	{
		if (LockLooper())
		{
			Invalidate(Bounds());
			UnlockLooper();
		}
	}
}

///////////////////////////////////////////////

OverlayWindow::OverlayWindow(BRect frame, OverlayView **outView)
: BWindow(frame, "overlay output", B_NO_BORDER_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, 0)
{
	AddChild(view = new OverlayView(frame.OffsetToCopy(0.,0.)));
	
	*outView = view;
	
	Hide();
	Show();
}

bool OverlayWindow::QuitRequested()
{
	return true;
}

void OverlayWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case OO_VISIBILITY_TOGGLE:
		{
			if (IsHidden())
			{
				// make sure we're fullscreen
				BRect scrFrm = BScreen(this).Frame();
				ResizeTo(scrFrm.Width(), scrFrm.Height());
			
				be_app->HideCursor();
				Show();
				view->MakeFocus(true);
			}
			else
			{
				Hide();
				be_app->ShowCursor();
			}
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}

