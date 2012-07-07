/*
	OverlayOut.cpp
	OverlayOut output plugin for Jeepers Elvis!
	2002 John Wiggins
*/

#include <interface/Bitmap.h>

//#include "JEPlugin.h"
#include "OverlayOut.h"
#include "OverlayUI.h"

// export foo
je_plugin overlay_plug = { JE_OUTPUT_PLUGIN, "Overlay Output", instantiate_overlay_output};

static je_plugin *plugins[] = {
	&overlay_plug,
	NULL
};

extern "C" _EXPORT je_plugin **get_plugins()
{
	return plugins;
}

// function defs
void * instantiate_overlay_output(void *arg)
{
	overlay_output_private *data = new overlay_output_private;
	je_output_plugin *me = new je_output_plugin;
//	status_t err;
	OverlayView *view;
	
	// fill in data
	data->vidWin = new OverlayWindow(BRect(0.,0.,639.,479.), &view);
	data->vidView = view;
		
	// check instantiation of video window
//	if (err < B_OK)
//	{
//		delete data->vidWin;
//		data->vidWin = NULL;
//		data->vidView = NULL;
//	}
	
	// construct the ui
	data->uiView = new OverlayUI(BRect(0.0,0.0,99.0,49.0), "ui", data->vidWin);

	// fill in the plugin struct
	me->data = data;
	me->output = overlay_output_output;
	me->free = overlay_output_free;
	me->getUI = overlay_output_getUI;
	
	return (void *)me;
}

void overlay_output_output(je_output_plugin *cookie, BBitmap *frame)
{
	overlay_output_private *self = (overlay_output_private *)cookie->data;
	
	if (self->vidView != NULL)
	{
		// easy
		self->vidView->UpdateBits(frame);
	}
}

void overlay_output_free(je_output_plugin *cookie)
{
	overlay_output_private *self = (overlay_output_private *)cookie->data;
	
	// delete the ui
	delete self->uiView;
	// lock & quit the video window
	if (self->vidWin != NULL)
	{
		self->vidWin->Lock();
		self->vidWin->Quit();
		
		// the window no longer exists (in theory)
		self->vidWin = NULL;
		self->vidView = NULL;
	}
}

BView *overlay_output_getUI(je_output_plugin *cookie)
{
	overlay_output_private *self = (overlay_output_private *)cookie->data;
	
	return self->uiView;
}
