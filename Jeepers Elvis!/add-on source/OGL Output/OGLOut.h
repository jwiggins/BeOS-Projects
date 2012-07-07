/*
	OGLOut.h
	OGLOut prototypes
	2002 John Wiggins
*/

#ifndef OGL_OUTPUT_H
#define OGL_OUTPUT_H

#include "JEPlugin.h"

void * instantiate_ogl_output(void *arg);
void ogl_output_output(je_output_plugin *cookie, BBitmap *frame);
void ogl_output_free(je_output_plugin *cookie);
BView *ogl_output_getUI(je_output_plugin *cookie);

// forward decls
class OGLUI;
class OGLWindow;

// plugin data
struct ogl_output_private {
	OGLUI *uiView;
	OGLWindow *vidWin;
};

#endif