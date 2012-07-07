#ifndef BFIV_SCROLL_VIEW_H
#define BFIV_SCROLL_VIEW_H

#ifndef	_VIEW_H
#include <View.h>
#endif
#ifndef _WINDOW_H
#include <Window.h>
#endif
#ifndef	_SCROLL_BAR_H
#include <ScrollBar.h>
#endif
#ifndef BFIV_VIEW_H
#include "BFIV_View.h"
#endif

class BFIV_ScrollView : public BView {
public:
										BFIV_ScrollView(BRect frame, BFIV_View *image);

//virtual	void				Draw(BRect update);
virtual	void				AllAttached();
virtual	void				FrameResized(float, float);
void								DoScrollBars();

private:
BFIV_View						*fImage;
};

#endif