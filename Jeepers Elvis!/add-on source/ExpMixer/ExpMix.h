/*
	ExpMix.h
	ExpMix class for Experimental Mixer mixer plugin (a Jeepers Elvis plugin)
	2000 John Wiggins
*/

#ifndef EXP_MIX_H
#define EXP_MIX_H

#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <posix/math.h>
#include <posix/stdio.h>

enum t_mix_op {
EXP_MIX_ADD = 0,
EXP_MIX_SUB,
EXP_MIX_MAX,
EXP_MIX_MIN
};

class ExpMix {
public:
				ExpMix();
				~ExpMix();
	
	void		Mix(BBitmap *back, BBitmap *fore, BBitmap *dst);
	void		SetMixOp(t_mix_op op);
	void		SetScale(BRect & src, BRect & dst);

private:
	
	t_mix_op	mOp;
	float		scaleX;
	float		scaleY;
};

#endif