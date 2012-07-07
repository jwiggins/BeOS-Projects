/*
	ChromaMix.h
	ChromaMix class for Chroma Key Mixer mixer plugin (a Jeepers Elvis plugin)
	2000 John Wiggins
*/

#ifndef CHROMA_MIX_H
#define CHROMA_MIX_H

#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <posix/math.h>
#include <posix/stdio.h>

class ChromaMix {
public:
				ChromaMix();
				~ChromaMix();
	
	void		Mix(BBitmap *back, BBitmap *fore, BBitmap *dst);
	void		SetKey(rgb_color key);
	void		SetScale(BRect & src, BRect & dst);

private:
	
	rgb_color	keyVal;
	float		scaleX;
	float		scaleY;
};

#endif