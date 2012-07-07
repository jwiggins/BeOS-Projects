/*
	OverlayOut.h
	OverlayOut prototypes
	2002 John Wiggins
*/

#ifndef OVERLAY_OUTPUT_H
#define OVERLAY_OUTPUT_H

#include "JEPlugin.h"

void * instantiate_overlay_output(void *arg);
void overlay_output_output(je_output_plugin *cookie, BBitmap *frame);
void overlay_output_free(je_output_plugin *cookie);
BView *overlay_output_getUI(je_output_plugin *cookie);

// forward decls
class OverlayUI;
class OverlayWindow;
class OverlayView;

// plugin data
struct overlay_output_private {
	OverlayUI *uiView;
	OverlayWindow *vidWin;
	OverlayView *vidView;
};

#endif