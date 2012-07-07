/*
	MovieLooper.h
	MovieLooper prototypes
	2000 John Wiggins
*/

#ifndef MOVIE_LOOPER_H
#define MOVIE_LOOPER_H

#include "JEPlugin.h"

void * movie_looper_instantiate(void *arg);
void movie_looper_start(je_source_plugin *cookie);
void movie_looper_stop(je_source_plugin *cookie);
void movie_looper_run(je_source_plugin *cookie);
void movie_looper_exit(je_source_plugin *cookie);
void movie_looper_free(je_source_plugin *cookie);
bool movie_looper_lock(je_source_plugin *cookie);
void movie_looper_unlock(je_source_plugin *cookie);
BView *movie_looper_getUI(je_source_plugin *cookie);
BBitmap *movie_looper_getFrame(je_source_plugin *cookie);
BRect movie_looper_outputSize(je_source_plugin *cookie);
bigtime_t movie_looper_nextFrameTime(je_source_plugin *cookie);

#endif