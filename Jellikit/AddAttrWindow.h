/*
	
	AddAttrWindow.h
	John Wiggins 1998
	
*/

#ifndef ADD_ATTR_WINDOW_H
#define ADD_ATTR_WINDOW_H

// interface kit
#include <Window.h>
#include <TextControl.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <Button.h>
#include <Font.h>
//app kit
#include <Messenger.h>
//storage kit
#include <Mime.h>
//ansi
#include <string.h>

// mine
#include "MessageDefs.h"
#include "AppResStrings.h"
#include "AppResStringDefs.h"
#include "AttrApp.h"

class AddAttrWindow : public BWindow {

public:
						AddAttrWindow(BRect frame, const char *title, BLooper *addon_manager, BLooper *attr_manager);
virtual void			MessageReceived(BMessage *msg);

private:
int32					current_window_id;
type_code				current_type;
size_t					current_size;
BMessenger				*AddonMessenger;
BMessenger				*AttributeMessenger;
void					BuildMimeMenu();
BMenu					*mime_menu;
BMenuField				*mime_menu_field;

};

#endif