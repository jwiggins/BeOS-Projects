/*
	SuperMix.h
	SuperMix class for Alpha Mixer mixer plugin (a Jeepers Elvis plugin)
	2000 John Wiggins
*/

#ifndef SUPER_MIX_H
#define SUPER_MIX_H

#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <posix/math.h>
#include <posix/stdio.h>

class SuperMix {
public:
				SuperMix();
				~SuperMix();
	
	void		Mix(BBitmap *back, BBitmap *fore, BBitmap *dst);
	void		SetAlpha(uint8 alf);
	void		SetScale(BRect & src, BRect & dst);

private:
	
	uint8		alpha;
	float		scaleX;
	float		scaleY;
};

#endif