/*
	WaveFilter.cpp
	WaveFilter filter plugin for Jeepers Elvis!
	2002 John Wiggins
*/

#include <interface/View.h>
#include <interface/StringView.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <support/SupportDefs.h>
#include <posix/math.h>
#include <posix/string.h>

//#include "JEPlugin.h"
#include "WaveFilter.h"

// Export foo
je_plugin wave_filter = {JE_FILTER_PLUGIN, "Wave Filter", instantiate_wave_filter};

static je_plugin *plugins[] = {
	&wave_filter,
	NULL
};

extern "C" _EXPORT je_plugin **get_plugins()
{
	return plugins;
}

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

#define SINE_TABLE_SIZE 64
#define WAVE_SCALE 8.0

// plugin data
struct wave_filter_private {
	EmptyView *uiView;
	BBitmap *dest;
	int8 sintab[SINE_TABLE_SIZE];
	int8 offset;
};

// function defs
void * instantiate_wave_filter(void *arg)
{
	wave_filter_private *data = new wave_filter_private;
	je_filter_plugin *me = new je_filter_plugin;
	
	// fill in data
	data->uiView = new EmptyView(BRect(0.0,0.0,75.0,50.0), "ui");
	data->offset = 0;
	data->dest = NULL;
	
	// init sine table (values in [-8,8]
	for (int i=0; i < SINE_TABLE_SIZE; i++)
		data->sintab[i] =
			(int8)(WAVE_SCALE*sin((double)i * 2.0 * M_PI / (double)SINE_TABLE_SIZE));

	// fill in the plugin struct
	me->data = data;
	me->filter = wave_filter_filter;
	me->free = wave_filter_free;
	me->getUI = wave_filter_getUI;
	
	return (void *)me;
}

void wave_filter_filter(je_filter_plugin *cookie, BBitmap *frame)
{
	wave_filter_private *self = (wave_filter_private *)cookie->data;
	uint32 *fBits = (uint32 *)frame->Bits();
	uint32 *dBits = NULL; // assign later in function
	//uint32 rowBuf[1024];
	uint32 val;
	int32 rowlen = frame->BytesPerRow() / 4;
	int32 width = frame->Bounds().IntegerWidth(), height = frame->Bounds().IntegerHeight();
	int32 nX = 0, nY = 0;
	
	// init the dest image buffer
	// or change it if the pipeline's image size changes
	// XXX : do some fucking error checking
	if (self->dest == NULL)
	{
		self->dest = new BBitmap(frame->Bounds(), frame->ColorSpace());
	}
	else if (self->dest->Bounds() != frame->Bounds())
	{
		delete self->dest;
		self->dest = new BBitmap(frame->Bounds(), frame->ColorSpace());
	}
	
	// init dBits
	dBits = (uint32 *)self->dest->Bits();
	
	// loop through the dst image
	for (int32 y = 0; y <= height; y++)
	{
		// zero the row
		//memset(rowBuf, 0, rowlen*4);
		
		// build the row
		for (int32 x = 0; x <= width; x++)
		{
			// calculate the shift for this pixel
			// x
			nX = x + self->sintab[(x+self->offset) % SINE_TABLE_SIZE];
			if (nX < 0)
				nX += width;
			else if (nX > width)
				nX %= width;
			// y
			nY = y + self->sintab[(y+self->offset) % SINE_TABLE_SIZE];
			if (nY < 0)
				nY += height;
			else if (nY > height)
				nY %= height;
		
			// source pixel
			val = fBits[(nY*rowlen)+nX];
			// assign (to row buffer)
			//rowBuf[x] = val;
			// assign
			dBits[(y*rowlen)+x] = val;
		}
		
		// assign the row
		//memcpy(&fBits[y*rowlen], rowBuf, rowlen*4);
	}
	
	// copy dest back to source
	memcpy(fBits, dBits, frame->BitsLength());
	
	// animate
	self->offset += 2;
	if (self->offset > SINE_TABLE_SIZE)
		self->offset %= SINE_TABLE_SIZE;
}

void wave_filter_free(je_filter_plugin *cookie)
{
	wave_filter_private *self = (wave_filter_private *)cookie->data;
	
	delete self->uiView;
	delete self->dest;
	delete self;
}

BView *wave_filter_getUI(je_filter_plugin *cookie)
{
	wave_filter_private *self = (wave_filter_private *)cookie->data;
	
	return self->uiView;
}
