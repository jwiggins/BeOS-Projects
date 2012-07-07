/*
	DebugView.h
	BView class for debugging Jeepers Elvis!
	2000 John Wiggins
*/

#ifndef DEBUG_VIEW_H
#define DEBUG_VIEW_H

#include <interface/View.h>
#include <interface/Rect.h>
#include <posix/stdio.h>

class DebugView : public BView {
public:
						DebugView(BRect frame, const char *name, uint32 resize, uint32 flags)
						: BView(frame, name, resize, flags)
						{
							printf("view \"%s\" = %p\n", name, this);
						};
	virtual	void		AttachedToWindow()
						{
							printf("AttachedToWindow() : (%p)\n", this);
						};
};

#endif