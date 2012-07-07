/*
	ChromaMix.cpp
	2000 John Wiggins
*/

#include "ChromaMix.h"

ChromaMix::ChromaMix()
{
	keyVal.red = 0;
	keyVal.green = 0;
	keyVal.blue = 255;
	keyVal.alpha = 0;
	
	scaleX = 1.0f;
	scaleY = 1.0f;
}

ChromaMix::~ChromaMix()
{
	// nichts
}

void ChromaMix::Mix(BBitmap *back, BBitmap *fore, BBitmap *dst)
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
		uint32 fRed, fGreen, fBlue;
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
		for (int32 y = 0; y < height; y++)
		{
			for (int32 x = 0; x < width; x++)
			{
				// do we need to scale?
				if (scale)
				{
					// foreground is scaled
					if (fScale)
					{
						fVal = fBits[(((int32)(y*scaleY))*fRowlen)+((int32)(x*scaleX))];
						bVal = bBits[(y*bRowlen)+x];
					}
					else // background is scaled
					{
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
				
				// compare with the key
				
				if (keyVal.blue > keyVal.red && keyVal.blue > keyVal.green && keyVal.blue > 127)
					if (fBlue > fRed && fBlue > fGreen && (fBlue > keyVal.blue))
						val = bVal;
					else
						val = fVal;
				else if (keyVal.green > keyVal.red && keyVal.green > keyVal.blue && keyVal.green > 127)
					if (fGreen > fRed && fGreen > fBlue && (fGreen > keyVal.green))
						val = bVal;
					else
						val = fVal;
				else if (keyVal.red > keyVal.green && keyVal.red > keyVal.blue && keyVal.red > 127)
					if (fRed > fGreen && fRed > fBlue && (fRed > keyVal.red))
						val = bVal;
					else
						val = fVal;
				else
					val = fVal;
				
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
		for (int32 y = 0; y < height; y++)
			for (int32 x = 0; x < width; x++)
				dBits[(y*rowlen)+x] = bBits[(y*bRowlen)+x];
	}
}

void ChromaMix::SetKey(rgb_color key)
{
	//printf("ChromaMix::SetKey()\n");
	keyVal = key;
}

void ChromaMix::SetScale(BRect & src, BRect & dst)
{
	//printf("ChromaMix::SetScale()\n");
	scaleX = src.Width() / dst.Width();
	scaleY = src.Height() / dst.Height();
}