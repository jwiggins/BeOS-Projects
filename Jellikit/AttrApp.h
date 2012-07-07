/*

	AttrApp.h - a generic attribute editor
	John Wiggins 1998
	
*/

#ifndef ATTR_APP_H
#define ATTR_APP_H

// App Kit
#include <Application.h>
#include <Message.h>
// Interface Kit
#include <Alert.h>
#include <Window.h>
#include <View.h>
#include <Rect.h>
#include <Screen.h>
// Support Kit
#include <List.h>
#include <TypeConstants.h>
// Storage Kit
#include <fs_attr.h> // actually in the kernel folder but who's keeping score....
#include <File.h>
#include <Node.h>
#include <Path.h>
#include <Entry.h>
#include <FilePanel.h>
// Other
#include <stdio.h>
#include <string.h>
// Mine
#include "AttrWindow.h"
#include "EditorWindow.h"
#include "AddAttrWindow.h"
#include "AboutWindow.h"
#include "AddonManager.h"
#include "AttributeManager.h"
#include "MessageDefs.h"
#include "AppResStrings.h"
#include "AppResStringDefs.h"

const char APP_SIGNATURE[] = "application/x.vnd-Prok.Jellikit";
#define WINDOW_X 309
#define WINDOW_Y 399

class EditorWindow;
class AttributeManager;
class AddonManager;

class AttrApp : public BApplication {
public:
									AttrApp();
virtual	void						ReadyToRun();
virtual void						RefsReceived(BMessage *msg);
virtual	bool						QuitRequested();
virtual void						MessageReceived(BMessage *msg);
virtual	void						AboutRequested();

BWindow	*							FindWindowByID(int32 window_id);
AppResStrings						*res_strings;
private:

// functions
BRect								GetNewWindowRect();
BWindow	*							FindWindow(const char *title);
void								SaveRequested(BMessage *msg);
status_t							Save(BMessage *msg); // does the actual save

// data
BFilePanel							*OpenPanel;
BFilePanel							*SaveAsPanel;
EditorWindow						*the_editor_window;
AddAttrWindow						*the_addattr_window;
AddonManager						*the_addon_manager;
AttributeManager					*the_attr_manager;
BMessenger							*AttrMessenger;
int32								window_count;
int32								window_id_source;
int32								next_untitled_number;
bool								about_box_alive, media_server_is_alive;

};

#endif
