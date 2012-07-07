#ifndef BFIV_PREF_WIN_H
#define BFIV_PREF_WIN_H

#ifndef _WINDOW_H
#include <Window.h>
#endif
#ifndef _VIEW_H
#include <View.h>
#endif
#ifndef _FONT_H
#include <Font.h>
#endif
#ifndef _TEXT_CONTROL_H
#include <TextControl.h>
#endif
#ifndef _BUTTON_H
#include <Button.h>
#endif
#ifndef _FILE_PANEL_H
#include <FilePanel.h>
#endif
#ifndef _PATH_H
#include <Path.h>
#endif
#ifndef BFIV_APP_H
#include "BFIV_App.h"
#endif
#ifndef MESSAGE_CONSTANTS_H
#include "MessageConstants.h"
#endif


class BFIV_PrefWin : public BWindow {
public:
								BFIV_PrefWin(BRect frame);
								~BFIV_PrefWin();
virtual bool		QuitRequested();
virtual	void		MessageReceived(BMessage *msg);

private:
BFilePanel			*filePanel;
};

#endif