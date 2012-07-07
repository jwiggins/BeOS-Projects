/*
	InputFilter.h
	InputFilter class for FullScreen Output (a Jeepers Elvis! plugin)
	2000 John Wiggins
*/

#ifndef INPUT_FILTER_H
#define INPUT_FILTER_H

#include <app/MessageFilter.h>
#include <app/Message.h>
#include <posix/stdio.h>

class InputFilter : public BMessageFilter {
public:
								InputFilter();
	
	virtual	filter_result		Filter(BMessage *msg, BHandler **target);
	
private:
};

#endif