/*

	EditorWindow.h
	John Wiggins 1998

*/

#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

// interface kit
#include <Window.h>
#include <InterfaceDefs.h>
#include <View.h>
#include <Button.h>
#include <Font.h>
// app kit
#include <Message.h>
// support kit
#include <SupportDefs.h>
#include <List.h>
// kernel kit
#include <image.h>
// ansi
#include <stdio.h>
#include <string.h>
// my includes
#include "MessageDefs.h"
#include "AttrAddon.h"
#include "AttributeManager.h"
#include "AttrApp.h"
#include "AppResStrings.h"
#include "AppResStringDefs.h"

class BView;
class BList;

typedef AttrAddon * (*attr_addon_instantiation_func) (BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret);

struct window_list_entry {
	int32		window_id;
	BView		*view;
	bool		ok_button_state;
	type_code	attr_type;
	char		*attr_name;
};

class EditorWindow : public BWindow {
public:
					EditorWindow(BRect frame);
					~EditorWindow();
virtual	bool		QuitRequested();
virtual	void		MessageReceived(BMessage *msg);

private:

// functions
void				SwapEditView(BMessage *msg);
void				SendAttributeToParent(AttrAddon *view);
void				AttrWindowActivated(int32 window_id);
void				ChangeWindowTitle();
window_list_entry	*GetListItem(int32 window_id);

// data
BList				*window_list; // keep track of the app's windows
BView				*background, *stub;
BButton				*ok_button;
BLooper				*attribute_manager;
int32				addon_frame_bottom;
type_code			current_attr_type;
char				*current_attr_name;
int32				current_window_id;
BView				*current_view;
};

#endif
