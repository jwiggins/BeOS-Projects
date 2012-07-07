/*

	AttrWindow.h
	John Wiggins 1998
	
*/

#ifndef ATTR_WINDOW_H
#define ATTR_WINDOW_H

// kit stuff
#include <Window.h>
#include <Rect.h>
#include <ScrollBar.h>
#include <ScrollView.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Message.h>
#include <Messenger.h>
#include <Looper.h>
#include <Alert.h>
#include <string.h>

// my stuff
#include "AttrApp.h"
#include "EditorWindow.h"
#include "AddAttrWindow.h"
#include "ColumnListView.h"
#include "AppResStrings.h"
#include "AppResStringDefs.h"

class EditorWindow;
class AddAttrWindow;
class BMessenger;
class BMessage;


class AttrWindow : public BWindow {
public:
					AttrWindow(BRect frame, const char *title, BMessage *config_msg, BLooper *addon_manager, BLooper *attribute_manager, EditorWindow *edit_window, AddAttrWindow *add_attr_window, int32 id);
virtual	bool		QuitRequested();
virtual	void		MessageReceived(BMessage *msg);
virtual	void		WindowActivated(bool active);

int32				ID() const {return _id_;}
void				EditItem(const char *name, image_id addon_id);

private:

// functions
int32				BuildMenubar(int32 width, int32 height);
void				Config(ColumnListView *list_view, BMessage *msg);
void				AddListItem(const char *name, type_code type, ColumnListView *list_view);
void				RemoveListItem(const char *name, ColumnListView *list_view);
void				FileTypeChanged(const char *new_type);

void				Register(bool need_id);
void				Unregister();

// data
int32				menu_bottom;
int32				_id_;
EditorWindow		*edit_win; // the window where the editing happens
AddAttrWindow		*add_attr_win; // a miniature attribute factory
BMessenger			*AddonMessenger, *AttributeMessenger;
bool				ok_to_quit;
};


#endif