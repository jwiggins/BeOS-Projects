#ifndef BFIV_WINDOW_H
#define BFIV_WINDOW_H

#ifndef _APPLICATION_H
#include <Application.h>
#endif
#ifndef _WINDOW_H
#include <Window.h>
#endif
#ifndef _ALERT_H
#include <Alert.h>
#endif
#ifndef _MENU_BAR_H
#include <MenuBar.h>
#endif
#ifndef _MENU_H
#include <Menu.h>
#endif
#ifndef _MENU_ITEM_H
#include <MenuItem.h>
#endif
#ifndef _SCROLL_BAR_H
#include <ScrollBar.h>
#endif
#ifndef _TRANSLATOR_H
#include <TranslationKit.h>
#endif
#ifndef _STORAGE_DEFS_H
#include <StorageDefs.h>
#endif
#ifndef MESSAGE_CONSTANTS_H
#include "MessageConstants.h"
#endif
#ifndef BFIV_STATUS_H
#include "BFIV_Status.h"
#endif
#ifndef BFIV_VIEW_H
#include "BFIV_View.h"
#endif
#ifndef BFIV_SCROLL_VIEW_H
#include "BFIV_ScrollView.h"
#endif
#ifndef BFIV_PREF_WIN_H
#include "BFIV_PrefWin.h"
#endif
#ifndef BFIV_APP_H
#include "BFIV_App.h"
#endif

class BFIV_Window : public BWindow {
public:
										BFIV_Window(BRect frame, const char *name);

virtual	void				MessageReceived(BMessage *msg);
virtual	bool				QuitRequested();

BMenuBar *					BuildMenus();

static int32				OpenThread(void *data);
int32								Open();

private:
BFIV_Status					*fStatus;
BFIV_View						*fImage;
BFIV_ScrollView			*fScroller;
BFIV_Tiler					*fFatBastard;
BScrollBar					*fHorScroller,*fVerScroller;
BMenu								*fScaleMenu;
entry_ref						fFile_ref;
thread_id						fReaderThid;
int32								fCurScale;

};

#endif