/*
	OGLOut.cpp
	OGLOut output plugin for Jeepers Elvis!
	2002 John Wiggins
*/

#include <interface/Bitmap.h>

//#include "JEPlugin.h"
#include "OGLOut.h"
#include "OGLUI.h"

// export foo
je_plugin ogl_plug = { JE_OUTPUT_PLUGIN, "OpenGL Output", instantiate_ogl_output};

static je_plugin *plugins[] = {
	&ogl_plug,
	NULL
};

extern "C" _EXPORT je_plugin **get_plugins()
{
	return plugins;
}

// function defs
void * instantiate_ogl_output(void *arg)
{
	ogl_output_private *data = new ogl_output_private;
	je_output_plugin *me = new je_output_plugin;
	
	// fill in data
	data->vidWin = new OGLWindow(BRect(0.,0.,639.,479.));
	
	// construct the ui
	data->uiView = new OGLUI(BRect(0.0,0.0,99.0,49.0), "ui", data->vidWin);

	// fill in the plugin struct
	me->data = data;
	me->output = ogl_output_output;
	me->free = ogl_output_free;
	me->getUI = ogl_output_getUI;
	
	return (void *)me;
}

void ogl_output_output(je_output_plugin *cookie, BBitmap *frame)
{
	ogl_output_private *self = (ogl_output_private *)cookie->data;
	
	if (self->vidWin != NULL)
	{
		// easy
		self->vidWin->SetTextureData(frame);
	}
}

void ogl_output_free(je_output_plugin *cookie)
{
	ogl_output_private *self = (ogl_output_private *)cookie->data;
	
	// delete the ui
	delete self->uiView;
	// lock & quit the video window
	if (self->vidWin != NULL)
	{
		self->vidWin->Lock();
		self->vidWin->Quit();
		
		// the window no longer exists (in theory)
		self->vidWin = NULL;
	}
}

BView *ogl_output_getUI(je_output_plugin *cookie)
{
	ogl_output_private *self = (ogl_output_private *)cookie->data;
	
	return self->uiView;
}
