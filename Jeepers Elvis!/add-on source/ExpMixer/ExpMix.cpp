/*
	ExpMix.cpp
	2002 John Wiggins
*/

#include "ExpMix.h"

ExpMix::ExpMix()
{
	mOp = EXP_MIX_ADD;
	
	scaleX = 1.0f;
	scaleY = 1.0f;
}

ExpMix::~ExpMix()
{
	// nichts
}

void ExpMix::Mix(BBitmap *back, BBitmap *fore, BBitmap *dst)
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
				// background components
				bRed = (bVal>>16 & 0xff);
				bGreen = (bVal>>8 & 0xff);
				bBlue = (bVal & 0xff);
				
#define MY_MAX(a,b) (a >= b ? a : b)
#define MY_MIN(a,b) (a <= b ? a : b) 
				
				// mix
				switch(mOp)
				{
					case EXP_MIX_ADD:
					{
						red = bRed + fRed;
						red %= 255;
						
						green = bGreen + fGreen;
						green %= 255;
						
						blue = bBlue + fBlue;
						blue %= 255;
						
						break;
					}
					case EXP_MIX_SUB:
					{
						red = bRed - fRed;
						red = red > 0 ? red : 0;
						
						green = bGreen - fGreen;
						green = green > 0 ? green : 0;
						
						blue = bBlue - fBlue;
						blue = blue > 0 ? blue : 0;
						
						break;
					}
					case EXP_MIX_MAX:
					{						
						red = MY_MAX(bRed, fRed);
						green = MY_MAX(bGreen, fGreen);
						blue = MY_MAX(bBlue, fBlue);
						
						break;
					}
					case EXP_MIX_MIN:
					{
						red = MY_MIN(bRed, fRed);
						green = MY_MIN(bGreen, fGreen);
						blue = MY_MIN(bBlue, fBlue);
						
						break;
					}
					default:
					{
						break;
					}
				}
				
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

void ExpMix::SetMixOp(t_mix_op op)
{
	//printf("ChromaMix::SetKey()\n");
	mOp = op;
}

void ExpMix::SetScale(BRect & src, BRect & dst)
{
	//printf("ChromaMix::SetScale()\n");
	scaleX = src.Width() / dst.Width();
	scaleY = src.Height() / dst.Height();
}