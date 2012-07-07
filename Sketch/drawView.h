/*
	drawView.h
	John Wiggins 1998
	
*/

#ifndef DRAWVIEW_H
#define DRAWVIEW_H

#ifndef _VIEW_H
#include <View.h>
#endif

class drawView : public BView {
public:
					drawView(BRect frame);
virtual	void		AttachedToWindow();
void				draw(); // erases the drawing
void				up(); // move pen up
void				down(); // move pen down
void				left(); // move pen left
void				right(); // move pen right

private:
BRect				bounds;

};

#endif
