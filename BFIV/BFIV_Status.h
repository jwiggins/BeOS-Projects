#ifndef BFIV_STATUS_H
#define BFIV_STATUS_H

#ifndef	_VIEW_H
#include <View.h>
#endif
#ifndef _STRING_VIEW_H
#include <StringView.h>
#endif
#ifndef	_STATUS_BAR_H
#include <StatusBar.h>
#endif
#ifndef MESSAGE_CONSTANTS_H
#include "MessageConstants.h"
#endif
#ifndef BFIV_TILER_H
#include "BFIV_Tiler.h"
#endif

class BFIV_Status : public BView {
public:
									BFIV_Status(BRect frame, const char *name);
virtual	void			MessageReceived(BMessage *msg);

private:
BStringView				*fDrop;
BStatusBar				*fStatus;

};

#endif