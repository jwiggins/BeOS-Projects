/*
	CaptureCardSource.h
	CaptureCard Source prototypes
	2001 John Wiggins
*/

#ifndef CAPTURE_CARD_H
#define CAPTURE_CARD_H

#include "JEPlugin.h"

void * capture_card_instantiate(void *arg);
void capture_card_start(je_source_plugin *cookie);
void capture_card_stop(je_source_plugin *cookie);
void capture_card_run(je_source_plugin *cookie);
void capture_card_exit(je_source_plugin *cookie);
void capture_card_free(je_source_plugin *cookie);
bool capture_card_lock(je_source_plugin *cookie);
void capture_card_unlock(je_source_plugin *cookie);
BView *capture_card_getUI(je_source_plugin *cookie);
BBitmap *capture_card_getFrame(je_source_plugin *cookie);
BRect capture_card_outputSize(je_source_plugin *cookie);
bigtime_t capture_card_nextFrameTime(je_source_plugin *cookie);

#endif