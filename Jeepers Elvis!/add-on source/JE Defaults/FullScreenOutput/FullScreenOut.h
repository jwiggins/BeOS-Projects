/*
	FullScreenOut.h
	FullScreenOut prototypes
	2000 John Wiggins
*/

#ifndef FULLSCREEN_OUTPUT_H
#define FULLSCREEN_OUTPUT_H

#include "JEPlugin.h"

void * instantiate_fullscreen_output(void *arg);
void fullscreen_output_output(je_output_plugin *cookie, BBitmap *frame);
void fullscreen_output_free(je_output_plugin *cookie);
BView *fullscreen_output_getUI(je_output_plugin *cookie);

// forward decls
class FullScreenUI;
class VideoScreen;
class OverlayWindow;
class OverlayView;

// plugin data
struct fullscreen_output_private {
	FullScreenUI *uiView;
	VideoScreen *vidWin;
	OverlayWindow *ovlWin;
	OverlayView *ovlView;
	bool overlay;
};

#endif