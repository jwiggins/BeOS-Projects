/*
	
	PassControl.h
	John Wiggins 1999

*/

#ifndef PASSCONTROL_H
#define PASSCONTROL_H

#include <TextControl.h>
#include <Font.h>
#include <Rect.h>
#include <Message.h>

#include <string.h>

#include "myFilter.h"

#define MAGIC_SIZE 64 // this is the starting length of the "actual" buffer

class myFilter;

class PassControl : public BTextControl {
public:
					PassControl(BRect frame,
								const char *name,
								const char *label, 
								const char *initial_text, 
								BMessage *message /*,
								uint32 rmask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
								uint32 flags = B_WILL_DRAW | B_NAVIGABLE*/);
								// Same as Be's Constructor
virtual				~PassControl(); // deletes filter
virtual void			SetText(const char *text);
void					PopChar(); // deletes last character
void					PushChar(BMessage* msg); // adds a char to actual
const char			*actualText() const; // returns actual
private:

myFilter			*filter;
char					*actual;
int32				actual_size; // Current size of "actual" buffer
int32				length;
};

#endif
