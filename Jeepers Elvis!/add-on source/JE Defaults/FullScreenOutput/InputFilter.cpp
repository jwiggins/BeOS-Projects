/*
	InputFilter.cpp
	2000 John Wiggins
*/

#include "InputFilter.h"

InputFilter::InputFilter()
: BMessageFilter(B_PROGRAMMED_DELIVERY, B_REMOTE_SOURCE)
{
}

filter_result InputFilter::Filter(BMessage *msg, BHandler **target)
{
	filter_result result;
	
	switch (msg->what)
	{
		case B_KEY_DOWN:
		case B_KEY_UP:
		case B_MOUSE_MOVED:
		{
			result = B_DISPATCH_MESSAGE;
			break;
		}
		default:
		{
			result = B_SKIP_MESSAGE;
			break;
		}
	}
	
	return result;
}

