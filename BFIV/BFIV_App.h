#ifndef BFIV_APP_H
#define BFIV_APP_H

#ifndef _APPLICATION_H
#include <Application.h>
#endif
#ifndef _FIND_DIRECTORY_H
#include <FindDirectory.h>
#endif
#ifndef BFIV_WINDOW_H
#include "BFIV_Window.h"
#endif

class BFIV_App : public BApplication {
public:
									BFIV_App(const char *sig);

virtual	void			ReadyToRun();
virtual	void			RefsReceived(BMessage *msg);
virtual	void			MessageReceived(BMessage *msg);
virtual	void			AboutRequested();
virtual	bool			QuitRequested();

const char				*ScratchDirectory() const { return fScratchDir.String(); };

private:
int32							fWinCount;
BString						fScratchDir;
};

#endif