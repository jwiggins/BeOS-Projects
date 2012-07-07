/*
	InvertFilter.cpp
	InvertFilter filter plugin for Jeepers Elvis!
	2000 John Wiggins
*/

#include <interface/View.h>
#include <interface/StringView.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>

//#include "JEPlugin.h"
#include "InvertFilter.h"

// simple BView derived class
class EmptyView : public BView {
public:
						EmptyView(BRect frame, const char *name)
						: BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW)
						{
							// cache width and height
							width = frame.Width();
							height = frame.Height();
							
							// set view color
							SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
							
							// add the string view
							BRect rect((width/2.0) - 15.0,
										(height/2.0) - 5.0,
										(width/2.0) + 15.0,
										(height/2.0) + 5.0);
							AddChild(new BStringView(rect, "str", "No UI"));
						}
	
	virtual	void		GetPreferredSize(float *w, float *h)
						{
							*w = width;
							*h = height;
						}

private:
	float				width, height;
};

// plugin data
struct invert_filter_private {
	EmptyView *uiView;
};

// function defs
void * instantiate_invert_filter(void *arg)
{
	invert_filter_private *data = new invert_filter_private;
	je_filter_plugin *me = new je_filter_plugin;
	
	// fill in data
	data->uiView = new EmptyView(BRect(0.0,0.0,75.0,50.0), "ui");

	// fill in the plugin struct
	me->data = data;
	me->filter = invert_filter_filter;
	me->free = invert_filter_free;
	me->getUI = invert_filter_getUI;
	
	return (void *)me;
}

void invert_filter_filter(je_filter_plugin *cookie, BBitmap *frame)
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
			
			// invert
			red = 255 - red;
			green = 255 - green;
			blue = 255 - blue;
			
			// smoosh back together
			val = (uint32)( ((red<<16) | (green<<8) | blue) & 0x00ffffff);
			// assign
			fBits[(y*rowlen)+x] = val;
		}
	}
}

void invert_filter_free(je_filter_plugin *cookie)
{
	invert_filter_private *self = (invert_filter_private *)cookie->data;
	
	delete self->uiView;
	delete self;
}

BView *invert_filter_getUI(je_filter_plugin *cookie)
{
	invert_filter_private *self = (invert_filter_private *)cookie->data;
	
	return self->uiView;
}
