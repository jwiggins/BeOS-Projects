/*
	
	SketchWindow.cpp
	
	John Wiggins 1998
	
*/


#include <Application.h>
#include <string.h>
#include "SketchWindow.h"
#include "SketchView.h"


SketchWindow::SketchWindow(BRect frame)
	: BWindow(frame, "Etch-A-Sketch" B_UTF8_REGISTERED, B_TITLED_WINDOW,B_NOT_RESIZABLE | B_NOT_ZOOMABLE), format(NULL)
{
}

bool SketchWindow::QuitRequested()
{
	if(format != NULL)
		delete [] format; // delete your little char *
	be_app->PostMessage(B_QUIT_REQUESTED);
	
	return(TRUE);
}

void SketchWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case BLUR:
		{
			((SketchView *)FindView("SketchView"))->Blur();
			break;
		}
		case ANIMATED_ERASE:
		{
			// call SketchView's AnimatedErase(), which calls Erase()
			((SketchView *)FindView("SketchView"))->AnimatedErase();
			break;
		}
		case ERASE:
		{
			// call SketchView's Erase() 
			((SketchView *)FindView("SketchView"))->Erase();
			break;
		}
		case B_SAVE_REQUESTED:
		{
			((SketchView *)FindView("SketchView"))->SaveBitmap(msg);
			break;
		}
		case TRANSLATOR:
		{
			// whenever the user changes his mind about what format the picture should
			// be saved as, we get a message that tells us what the new format is.
			// we wanna cache a copy of this for use at the appropriate time
			const char *mime;
			msg->FindString("save format", &mime);
			if(format != NULL)
			{
				delete [] format;
				format = new char [ strlen(mime) + 1];
				strcpy(format, mime);
			}
			else
			{
				format = new char [ strlen(mime) + 1];
				strcpy(format, mime);
			}
			break;
		}
		default:
			BWindow::MessageReceived(msg);
			break;
	}
}

void SketchWindow::FrameMoved(BPoint)
{
	// spawn a thread for this!!!
	if(find_thread("Shake dis window") < 0) // make sure the thread isnt there yet
	{
		// basic threading, look it up in the BeBook
		resume_thread(spawn_thread(thread_start, "Shake dis window", B_NORMAL_PRIORITY, (void *)this));
	}
}

int32 SketchWindow::thread_start(void *arg)
{
	// this gives our thread access to a single global instatiation of the class 
	// (im pretty sure thats whats going on :)
	SketchWindow *obj = (SketchWindow *)arg;
	return (obj->thread_func());
}

int32 SketchWindow::thread_func()
{
	// we do the work here
	uint32	buttons =0;
	int32	ups=0, downs=0, lefts=0, rights=0;
	BPoint	reference, new_position;
	bool	erasing=false; // if this movement isnt serious, then dont erase
	
	// you must lock when calling GetMouse
	if(Lock())
	{
		((SketchView *)FindView("SketchView"))->GetMouse(&reference, &buttons, false);
		((SketchView *)FindView("SketchView"))->ConvertToScreen(&reference);
		Unlock();
	}
	while( buttons )
	{
		// check for sufficient movement
		// (side to side AND up and down)
		if((new_position.y + 5) < reference.y)
			ups++;
		else if((new_position.y - 5) > reference.y)
			downs++;
		if((new_position.x + 5) < reference.x)
			lefts++;
		else if((new_position.x - 5) > reference.x)
			rights++;
		
		// if we're getting moved enough, blur a little bit
		if((ups > 2) && (downs > 2))
		{
			PostMessage(BLUR); // reduce the likelyhood of deadlocks by letting the 
								// kits Lock()/Unlock() for you.
			if(!erasing) // if it hasnt already been found that we are indeed erasing, make it known
				erasing = true;
			ups = downs = 0;
		}
		if((lefts > 2) && (rights > 2))
		{
			PostMessage(BLUR); // see above explanation ^
			if(!erasing)
				erasing = true;
			lefts = rights = 0;
		}
		snooze( 75 * 1000 ); // sleep for a bit so you don't hog the cpu (bad)
		// lock and take another look at the mouse
		if(Lock())
		{
			((SketchView *)FindView("SketchView"))->GetMouse(&new_position, &buttons, false);
			((SketchView *)FindView("SketchView"))->ConvertToScreen(&new_position);
			Unlock();
		}
	}
	
	// comment this if() out for a different effect
	if(erasing)
	{
		// they were shaking the window sufficiently, so finish up
		// this is just in case they drop the window prematurely
		PostMessage(ERASE);
		return B_OK;
	}
	
	return B_OK;
}

