/*
	WhackerFilt.cpp
	2000 John Wiggins
*/

#include "WhackerFilt.h"

WhackerFilt::WhackerFilt()
{
//	redChan = true;
//	greenChan = true;
//	blueChan = true;
	redChan = greenChan = blueChan = 1.0;
}

void WhackerFilt::Filter(BBitmap *frame)
{
	uint32 *fBits = (uint32 *)frame->Bits();
	uint32 val, red, green, blue;
	int32 rowlen = frame->BytesPerRow() / 4;
	int32 width = frame->Bounds().IntegerWidth(), height = frame->Bounds().IntegerHeight();
	
	// loop through the dst image
	for (int32 y = 0; y <= height; y++)
	{
		for (int32 x = 0; x <= width; x++)
		{
			val = fBits[(y*rowlen)+x];
			
			// extract components
			red = (val>>16 & 0xff);
			green = (val>>8 & 0xff);
			blue = (val & 0xff);
			
			// clear val
			val = 0;
//			// OR in channels that are "on"
//			if (redChan)
//				val |= (red<<16) & 0x00ff0000;
//			if (greenChan)
//				val |= (green<<8) & 0x0000ff00;
//			if (blueChan)
//				val |= blue & 0x000000ff;
			// Add in channels
			val += (((uint32)(red*redChan))<<16) & 0x00ff0000;
			val += (((uint32)(green*greenChan))<<8) & 0x0000ff00;
			val += ((uint32)(blue*blueChan)) & 0x000000ff;
			
			// assign
			fBits[(y*rowlen)+x] = val;
		}
	}
}

