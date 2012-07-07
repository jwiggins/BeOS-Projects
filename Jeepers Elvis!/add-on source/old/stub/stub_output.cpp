/*
	stub_output.cpp
	2000 John Wiggins
*/

#include "stub_output.h"
#include "DebugView.h"

struct output_private {
	BView *uiView;
};

void * instantiate_stub_output(void *arg)
{
	output_private *data = new output_private;
	je_output_plugin *me = new je_output_plugin;
	
	// fill in data
	data->uiView = new DebugView(BRect(0.0,0.0,74.0,49.0), "output", B_FOLLOW_ALL, B_WILL_DRAW);

	// fill in me
	me->output = stub_output_output;
	me->free = stub_output_free;
	me->getUI = stub_output_getui;
	me->data = data;
	
	return (void *)me;
}

void stub_output_output(je_output_plugin *cookie, BBitmap *frame)
{
}

void stub_output_free(je_output_plugin *cookie)
{
	output_private *data = (output_private *)cookie->data;
	
	delete data->uiView;
	delete data;
}

BView *stub_output_getui(je_output_plugin *cookie)
{
	output_private *data = (output_private *)cookie->data;
	
	return data->uiView;
}
