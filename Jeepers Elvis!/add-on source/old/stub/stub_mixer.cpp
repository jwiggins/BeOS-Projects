/*
	stub_mixer.cpp
	2000 John Wiggins
*/

#include "stub_mixer.h"
#include "DebugView.h"

struct mixer_private {
	BView *uiView;
};

void * instantiate_stub_mixer(void *arg)
{
	mixer_private *data = new mixer_private;
	je_mixer_plugin *me = new je_mixer_plugin;
	
	// fill in data
	data->uiView = new DebugView(BRect(0.0,0.0,74.0,49.0), "mixer", B_FOLLOW_ALL, B_WILL_DRAW);

	// fill in me
	me->mix = stub_mixer_mix;
	me->free = stub_mixer_free;
	me->getOutputSize = stub_mixer_getoutputsize;
	me->getUI = stub_mixer_getui;
	me->data = data;
	
	return (void *)me;
}

void stub_mixer_mix(je_mixer_plugin *cookie, BBitmap *src1, BBitmap *src2, BBitmap *dst)
{
}

void stub_mixer_free(je_mixer_plugin *cookie)
{
	mixer_private *data = (mixer_private *)cookie->data;
	
	delete data->uiView;
	delete data;
}

BRect stub_mixer_getoutputsize(je_mixer_plugin *cookie, BRect src1, BRect src2)
{
	return src1;
}

BView *stub_mixer_getui(je_mixer_plugin *cookie)
{
	mixer_private *data = (mixer_private *)cookie->data;
	
	//printf("stub_mixer_getui(): uiView = %p\n", data->uiView);
	
	return data->uiView;
}
