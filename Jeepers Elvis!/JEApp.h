/*
	JEApp.h
	BApplication class declaration for Jeepers Elvis!
	2000 John Wiggins
*/

#ifndef JE_APP_H
#define JE_APP_H

#include <app/Application.h>
#include <app/Invoker.h>
#include <interface/Alert.h>
#include <support/String.h>
#include <posix/stdlib.h>
#include <posix/assert.h>

#include "SourceThread.h"
#include "VideoWindow.h"
#include "MainWindow.h"
#include "PluginListWindow.h"
#include "PluginManager.h"
#include "GraphicsPipeline.h"
#include "AppResStrings.h"
#include "MessageConstants.h"
#include "JEPlugin.h"

class JEApp : public BApplication {
public:
						JEApp(const char *sig);
						~JEApp();
	
	virtual	void		ReadyToRun();
	virtual	void		MessageReceived(BMessage *msg);
	virtual	void		AboutRequested();
	virtual	bool		QuitRequested();
	
	const char	*		ResString(int32 id) const { return resStrings.String(id); };
private:
	
	void				SendMeEmail();
	const char 	*		RandomQuote();

	SourceThread		*source1, *source2;
	VideoWindow			*source1Win, *source2Win, *defaultOut;
	MainWindow			*mainWin;
	PluginListWindow	*piWindow;
	PluginManager		*piManager;
	GraphicsPipeline	*graphPipe;
	AppResStrings		resStrings;
	int32				windowCount;
	bool				postedQuit;
};

#endif