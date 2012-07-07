/*
	AlphaMixer.h
	Alpha Mixer prototypes
	2000 John Wiggins
*/

#ifndef ALPHA_MIXER_H
#define ALPHA_MIXER_H

#include "JEPlugin.h"

void * instantiate_alpha_mixer(void *arg);
void alpha_mixer_mix(je_mixer_plugin *cookie, BBitmap *src1, BBitmap *src2, BBitmap *dst);
void alpha_mixer_free(je_mixer_plugin *cookie);
BRect alpha_mixer_getOutputSize(je_mixer_plugin *cookie, BRect src1, BRect src2);
BView *alpha_mixer_getUI(je_mixer_plugin *cookie);

#endif