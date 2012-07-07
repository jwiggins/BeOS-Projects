/*
	
	SketchWindow.h
	
	John Wiggins 1998
	
*/

#ifndef SKETCH_WINDOW_H
#define SKETCH_WINDOW_H

#include <Window.h>
//#include <stdio.h>
//#include <OS.h>

// my Message constants
enum {
	ERASE = 'eras',
	ANIMATED_ERASE = 'aner',
	SAVE_BITMAP = 'save',
	TRANSLATOR = 'tran',
	BLUR = 'blur'
};

class SketchWindow : public BWindow {

public:
						SketchWindow(BRect frame);
virtual	void			MessageReceived(BMessage *msg);
virtual void			FrameMoved(BPoint); // spawns a window watching thread 
											//as soon as it findd out that the window is moving
virtual	bool			QuitRequested();

int32					thread_func(); // has the actual code for the thread in it
static	int32			thread_start(void *arg); // makes sure the thread has a this pointer

char 					*format; // format to save as

private:

};

#endif
