/*
	stub_filter.cpp
	2000 John Wiggins
*/

#include "stub_filter.h"
#include "DebugView.h"

struct filter_private {
	BView *uiView;
};

void * instantiate_stub_filter(void *arg)
{
	filter_private *data = new filter_private;
	je_filter_plugin *me = new je_filter_plugin;
	
	// fill in data
	data->uiView = new DebugView(BRect(0.0,0.0,74.0,49.0), "filter", B_FOLLOW_ALL, B_WILL_DRAW);
	
	// fill in me
	me->filter = stub_filter_filter;
	me->free = stub_filter_free;
	me->getUI = stub_filter_getui;
	me->data = (void *)data;
	
	return (void *)me;
}

void stub_filter_filter(je_filter_plugin *cookie, BBitmap *frame)
{
}

void stub_filter_free(je_filter_plugin *cookie)
{
	filter_private *data = (filter_private *)cookie->data;
	
	delete data->uiView;
	delete data;
}

BView *stub_filter_getui(je_filter_plugin *cookie)
{
	filter_private *data = (filter_private *)cookie->data;
	
	return data->uiView;
}
