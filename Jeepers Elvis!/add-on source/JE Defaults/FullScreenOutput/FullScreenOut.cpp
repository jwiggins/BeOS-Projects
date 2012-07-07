/*
	FullScreenOut.cpp
	FullScreenOut output plugin for Jeepers Elvis!
	2000 John Wiggins
*/

#include <interface/Bitmap.h>

//#include "JEPlugin.h"
#include "FullScreenOut.h"
#include "FullScreenUI.h"
#include "VideoScreen.h"
#include "OverlayWindow.h"

// export foo
//je_plugin fullscreen_plug = { JE_OUTPUT_PLUGIN, "FullScreen Output", instantiate_fullscreen_output};
//
//static je_plugin *plugins[] = {
//	&fullscreen_plug,
//	NULL
//};
//
//extern "C" _EXPORT je_plugin **get_plugins()
//{
//	return plugins;
//}

// function defs
void * instantiate_fullscreen_output(void *arg)
{
	fullscreen_output_private *data = new fullscreen_output_private;
	je_output_plugin *me = new je_output_plugin;
	status_t err;
	OverlayView *view;
	
	// fill in data
	data->ovlWin = new OverlayWindow(BRect(0.,0.,639.,479.), &view);
	data->ovlView = view;
	data->overlay = false;
	data->vidWin = new VideoScreen(B_32_BIT_640x480, &err);
		
	// check instantiation of video window
	if (err < B_OK)
	{
		delete data->vidWin;
		data->vidWin = NULL;
	}
	
	// construct the ui
	data->uiView = new FullScreenUI(BRect(0.0,0.0,199.0,99.0), "ui", data);

	// fill in the plugin struct
	me->data = data;
	me->output = fullscreen_output_output;
	me->free = fullscreen_output_free;
	me->getUI = fullscreen_output_getUI;
	
	return (void *)me;
}

void fullscreen_output_output(je_output_plugin *cookie, BBitmap *frame)
{
	fullscreen_output_private *self = (fullscreen_output_private *)cookie->data;
	
	if (self->overlay)
	{
		// overlay
		// easy
		self->ovlView->UpdateBits(frame);
	}
	else if (self->vidWin != NULL)
	{
		// BWindowScreen
		// easy
		self->vidWin->Draw(frame);
	}
}

void fullscreen_output_free(je_output_plugin *cookie)
{
	fullscreen_output_private *self = (fullscreen_output_private *)cookie->data;
	
	// delete the ui
	delete self->uiView;
	// lock & quit the video window
	if (self->vidWin != NULL)
	{
		self->vidWin->Lock();
		self->vidWin->Quit();
		
		self->ovlWin->Lock();
		self->ovlWin->Quit();
		
		// the windows no longer exist (in theory)
		self->vidWin = NULL;
		self->ovlWin = NULL;
	}
}

BView *fullscreen_output_getUI(je_output_plugin *cookie)
{
	fullscreen_output_private *self = (fullscreen_output_private *)cookie->data;
	
	return self->uiView;
}
