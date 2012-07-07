/*
	PluginMessages.h
	BMessage::what constants for Jeepers Elvis! plugins
	2000 John Wiggins
*/

#ifndef PLUGIN_MESSAGES_H
#define PLUGIN_MESSAGES_H

enum {
	JE_DRAW_BITMAP = 'draw',
	JE_FRAMERATE_CHANGED = 'fmrt',
	JE_FRAME_SIZE_CHANGED = 'fmsz',
	JE_SOURCE_READY = 'scrd'
};

/*
	Fields found in above messages:

	JE_DRAW_BITMAP: (JE_SOURCE_PLUGIN)
		- "bitmap" : a pointer to a BBitmap. Receiver should blit the bitmap
		to the screen. (or some other suitable device :)
	JE_FRAMERATE_CHANGED: (JE_SOURCE_PLUGIN)
		- "framerate" : a bigtime_t representing the new framerate. Value
		reflects the lifespan of a frame. (ex: 33000 would be ~30 fps)
		- "which" : an int32 containing the source's id.
	JE_FRAME_SIZE_CHANGED: (JE_SOURCE_PLUGIN)
		- "which" : an int32 containing the source's id.
	JE_SOURCE_READY: (JE_SOURCE_PLUGIN)
		- "which" : an int32 containing the source's id.
*/

#endif