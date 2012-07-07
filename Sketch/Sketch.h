/*
	
	Sketch.h
	John Wiggins 1998

*/

#ifndef SKETCH_H
#define SKETCH_H

#include <Application.h>
#include <TranslationKit.h>
#include <FilePanel.h>
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <Alert.h>
#include <ClassInfo.h>
#include "SketchWindow.h"


class SketchApp : public BApplication {

public:
					SketchApp();
virtual	void		AboutRequested();
virtual	void		MessageReceived(BMessage *msg);
virtual	void		ReadyToRun();
BWindow	* 			FindWindow(const char *title);
BFilePanel	*		FindFilePanel();

private:
BFilePanel			*saver;
};

#endif
