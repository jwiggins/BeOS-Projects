/*
	SuperMix.cpp
	2000 John Wiggins
*/

#include "SuperMix.h"

SuperMix::SuperMix()
{
	alpha = 127;
	scaleX = 1.0f;
	scaleY = 1.0f;
}

SuperMix::~SuperMix()
{
	// nichts
}

void SuperMix::Mix(BBitmap *back, BBitmap *fore, BBitmap *dst)
{
	uint32 *dBits = (uint32 *)dst->Bits();
	int32 rowlen = dst->BytesPerRow() / 4;
	int32 width = dst->Bounds().IntegerWidth(), height = dst->Bounds().IntegerHeight();
	
	// if there are two sources
	if (fore != NULL)
	{
		BRect backRect = back->Bounds(), foreRect = fore->Bounds();
		uint32 *fBits = (uint32 *)fore->Bits(), *bBits = (uint32 *)back->Bits();
		int32 fRowlen = fore->BytesPerRow() / 4, bRowlen = back->BytesPerRow() / 4;
		uint32 fVal, bVal, val;
		uint32 fRed, fGreen, fBlue, bRed, bGreen, bBlue, red, green, blue;
		bool scale = false, bScale = false, fScale = false;
		
		// find out who needs to be scaled (if any)
		if (backRect.Width() < foreRect.Width() || backRect.Height() < foreRect.Height())
		{
			scale = true;
			bScale = true;
		}
		else if (backRect.Width() > foreRect.Width() || backRect.Height() > foreRect.Height())
		{
			scale = true;
			fScale = true;
		}
		
		// loop through the dst image
		for (int32 y = 0; y <= height; y++)
		{
			for (int32 x = 0; x <= width; x++)
			{
				// do we need to scale?
				if (scale)
				{
					// foreground is scaled
					if (fScale)
					{
						//fVal = fBits[(((int32)floor(y*scaleY))*fRowlen)+((int32)floor(x*scaleX))];
						fVal = fBits[(((int32)(y*scaleY))*fRowlen)+((int32)(x*scaleX))];
						bVal = bBits[(y*bRowlen)+x];
					}
					else // background is scaled
					{
						//bVal = bBits[(((int32)floor(y*scaleY))*bRowlen)+((int32)floor(x*scaleX))];
						bVal = bBits[(((int32)(y*scaleY))*bRowlen)+((int32)(x*scaleX))];
						fVal = fBits[(y*fRowlen)+x];
					}
				}
				else
				{
					fVal = fBits[(y*fRowlen)+x];
					bVal = bBits[(y*bRowlen)+x];
				}
				
				// foreground components
				fRed = (fVal>>16 & 0xff);
				fGreen = (fVal>>8 & 0xff);
				fBlue = (fVal & 0xff);
				// background components
				bRed = (bVal>>16 & 0xff);
				bGreen = (bVal>>8 & 0xff);
				bBlue = (bVal & 0xff);
				// blend
				// (http://www.gamedev.net/reference/articles/article817.asp)
				red = ((alpha*(fRed-bRed)) >> 8) + bRed;
				green = ((alpha*(fGreen-bGreen)) >> 8) + bGreen;
				blue = ((alpha*(fBlue-bBlue)) >> 8) + bBlue;
				// smoosh back together
				val = (uint32)( ((red<<16) | (green<<8) | blue) & 0x00ffffff);
				// assign
				dBits[(y*rowlen)+x] = val;
			}
		}
	}
	else // only one source
	{
		uint32 *bBits = (uint32 *)back->Bits();
		int32 bRowlen = back->BytesPerRow() / 4;
		
		// copy back into dst
		// back and dst guaranteed to be the same size
		for (int32 y = 0; y <= height; y++)
			for (int32 x = 0; x <= width; x++)
				dBits[(y*rowlen)+x] = bBits[(y*bRowlen)+x];
	}
}

void SuperMix::SetAlpha(uint8 alf)
{
	//printf("SuperMix::SetAlpha()\n");
	alpha = alf;
}

void SuperMix::SetScale(BRect & src, BRect & dst)
{
	//printf("SuperMix::SetScale()\n");
	scaleX = src.Width() / dst.Width();
	scaleY = src.Height() / dst.Height();
}