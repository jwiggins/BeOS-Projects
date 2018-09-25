/*
	
	myFilter.h
	John Wiggins 1999

*/

#ifndef MYFILTER_H
#define MYFILTER_H

#include <MessageFilter.h>
#include <Message.h>
#include <Handler.h>
#include <InterfaceDefs.h>
#include <SupportDefs.h>

#include <stdio.h>

#include "PassControl.h"


class myFilter : public BMessageFilter {
public:
						myFilter();
virtual filter_result	Filter(BMessage *message, BHandler **target);

};
#endif
