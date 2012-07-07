/*
	MLMessages.h
	Message what constants for Movie Looper (a Jeepers Elvis plugin)
	2000 John Wiggins
*/

#ifndef ML_MESSAGES_H
#define ML_MESSAGES_H

enum {
	ML_SET_START_POINT = 'sspt',
	ML_SET_END_POINT = 'sept',
	ML_SET_INDICATOR_POSITION = 'sidp',
	ML_SET_PLAY_DIRECTION = 'spld',
	ML_LOAD_VIDEO_CLIP = 'lvid',
};

#endif

/*
	Message Contents:
	
	ML_SET_START_POINT:
		- "where" : an int64 specifying a starting frame
	ML_SET_END_POINT:
		- "where" : an int64 specifying an ending frame
	ML_SET_INDICATOR_POSITION:
		- "where" : an int64 specifying the current frame
	ML_SET_PLAY_DIRECTION:
		- "dir" : an int32 representing 1 of 3 play directions
	ML_LOAD_VIDEO_CLIP:
		- "view" : a pointer to the list view of clips
*/