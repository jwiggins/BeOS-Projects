/*

	AttrWindow.cpp
	John Wiggins 1998

*/

#include "AttrWindow.h"
#include "AttrItem.h"

AttrWindow::AttrWindow(BRect frame, const char *title, BMessage *config_msg, BLooper *addon_manager, BLooper *attribute_manager, EditorWindow *edit_window, AddAttrWindow *add_attr_window, int32 id)
			: BWindow(frame, title, B_DOCUMENT_WINDOW_LOOK,B_NORMAL_WINDOW_FEEL,0), _id_(id), ok_to_quit(false)
{
	BRect		rect, bounds = frame;
	int32		width = (int32)bounds.Width();
	BScrollView *LeftScrollView;
	ColumnListView *listview;
	const char *string_ptr;
	
	// init the addon manager messenger & attribute manager messenger
	AddonMessenger = new BMessenger(NULL, addon_manager, NULL);
	AttributeMessenger = new BMessenger(NULL, attribute_manager, NULL);
	
	// point at the editor window. it doesn't mind at all.
	edit_win = edit_window;
	add_attr_win = add_attr_window;
	
	// Register with your local loopers
	if(!strcmp(title, B_EMPTY_STRING))
		Register(true);
	else
		Register(false);
	
	bounds.OffsetTo(B_ORIGIN);
	// add a menubar	
	menu_bottom = BuildMenubar(width,bounds.Height());
	
	//config_msg->PrintToStream();
	
	// add the AttrListView and its scroller
	rect.Set(0, menu_bottom, width - B_V_SCROLL_BAR_WIDTH, bounds.bottom - B_H_SCROLL_BAR_HEIGHT);
	listview = new ColumnListView(rect,&LeftScrollView,"Attribute List",B_FOLLOW_ALL_SIDES,
		B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE,B_SINGLE_SELECTION_LIST,false,true,true,B_NO_BORDER);
	listview->AddColumn(new CLVColumn(B_EMPTY_STRING,20.0,CLV_LOCK_WITH_RIGHT|CLV_MERGE_WITH_RIGHT,15.0));
	
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_ATTR_NAME);
	listview->AddColumn(new CLVColumn(string_ptr,200.0,0,95.0));
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_TYPE);
	listview->AddColumn(new CLVColumn(string_ptr,70.0,0,70.0));
	
	// config
	Config(listview, config_msg); // add items for each attribute in the message
	AddChild(LeftScrollView);
	
	// finish up
	SetSizeLimits(150, 1<<20, 150, 1<<20);
	
	Show();
}

bool AttrWindow::QuitRequested()
{
	if(!ok_to_quit)
	{
		BMessage save_check(IS_FILE_DIRTY), reply;
	
		save_check.AddInt32("win id", _id_);
		AttributeMessenger->SendMessage(&save_check, &reply);
		//reply.PrintToStream();
		if(reply.what == ANSWER_YES)
		{
			// save
			BAlert *alert;
			const char *string_ptr1, *string_ptr2, *string_ptr3;
			char *allocd_string;
			
			string_ptr1 = ((AttrApp *)be_app)->res_strings->String(STRING_SAVE_BEFORE_CLOSE);
			string_ptr2 = Title();
			
			allocd_string = (char *)malloc(strlen(string_ptr1) + strlen(string_ptr2) + 1);
			sprintf(allocd_string, string_ptr1, string_ptr2);
			
			string_ptr1 = ((AttrApp *)be_app)->res_strings->String(STRING_CANCEL);
			string_ptr2= ((AttrApp *)be_app)->res_strings->String(STRING_DONT_SAVE);
			string_ptr3 = ((AttrApp *)be_app)->res_strings->String(STRING_OK);
			
			alert = new BAlert("alert", allocd_string, string_ptr1, string_ptr2, string_ptr3, B_WIDTH_FROM_WIDEST, B_OFFSET_SPACING, B_WARNING_ALERT);
			alert->SetShortcut(0,B_ESCAPE);
			alert->Go(new BInvoker(new BMessage(SAVE_BEFORE_QUITTING), this));
			
			free(allocd_string);
			return false;
		}
	}
	
	Unregister();
	return BWindow::QuitRequested();
}

void AttrWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case WINDOW_ADDED:
		{
			const char *string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_UNTITLED);
			char *s;
			int32 id = 0;
			if (msg->FindInt32("new_window_number", &id) == B_OK)
			{
				s = (char *)malloc(strlen(string_ptr) + 16 + 1);
				sprintf(s, "%s %ld", string_ptr, id);
				SetTitle(s);
				free(s);
			}
			break;
		}
		case WINDOW_SET_TITLE:
		{
			// msg contains:
			// "title" - new title for this window - string
			// action:
			// - SetTitle()
			const char *title;
			
			if(msg->FindString("title", &title) == B_NO_ERROR)
				SetTitle(title);
			break;
		}
		case BEOS_TYPE_CHANGE:
		{
			// msg contains:
			// "new type" - string - new mimestring for file
			const char *new_type;
			
			//printf("Got BEOS_TYPE_CHANGE msg\n");
			if(msg->FindString("new type", &new_type) == B_NO_ERROR)
				FileTypeChanged(new_type);
			break;
		}
		case REMOVE_ATTRIBUTE:
		{
			BListView *listview = (BListView *)FindView("Attribute List");
			
			if(listview != NULL)
			{
				AttrItem *selected_item = (AttrItem *)listview->RemoveItem(listview->CurrentSelection());
				if(selected_item != NULL)
				{
					// delete the fucker
					const char *attr_name = selected_item->AttrName();
					BMessage command(REMOVE_ATTRIBUTE);
					
					command.AddInt32("win id", _id_);
					command.AddString("attr name", attr_name);
					
					AttributeMessenger->SendMessage(&command);
					
					delete selected_item;
				}
			}
			break;
		}
		case DRAGGED_ATTRIBUTE:
		{
			// an attribute was dropped on us
			// msg feilds are "name", "type", "addon?", "addon id", "win id"
			const char *name, *type;
			int32 win_id, addon_id;
			bool has_addon;
			
			//msg->PrintToStream();
			if(msg->FindString("name", &name) == B_NO_ERROR)
				if(msg->FindString("type", &type) == B_NO_ERROR)
					if(msg->FindBool("addon?", &has_addon) == B_NO_ERROR)
						if(msg->FindInt32("addon id", &addon_id) == B_NO_ERROR)
							if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
								if(win_id != _id_) // can't drag to self!
								{
									BMessage command(DUPLICATE_ATTRIBUTE);
									command.AddInt32("dest win id", _id_); // my window id
									command.AddInt32("src win id", win_id); // their window id
									command.AddInt32("addon id", addon_id);
									command.AddString("type", type);
									command.AddString("attr name", name);
									command.AddBool("addon?", has_addon);
									command.AddBool("overwrite", false);
									AttributeMessenger->SendMessage(&command); // let the attribute manager take care of it
									// we will get a message telling us to add an attribute to the list
								}
							
			break;
		}
		case DRAGGED_ATTRIBUTE_LISTADD:
		{
			// msg contains:
			// "existed" - bool - 'true' means that the dragged attribute replaced an attribute of the same name.
			// remove the old attribute from the list and add the new one
			const char *name, *type;
			bool replace = false, has_addon;
			int32 addon_id;
			ColumnListView *list_view = static_cast<ColumnListView *>(FindView("Attribute List"));
			
			if(msg->FindString("attr name", &name) == B_NO_ERROR)
				if(msg->FindString("type", &type) == B_NO_ERROR)
					if(msg->FindBool("addon?", &has_addon) == B_NO_ERROR)
						if(msg->FindInt32("addon id", &addon_id) == B_NO_ERROR)
							if(msg->FindBool("overwrite", &replace) == B_NO_ERROR)
							{
								if(replace)
								{
									//printf("replacing attr: %s\n", name);
									RemoveListItem(name, list_view);
									//printf("attr: %s all removed.\n", name);
								}
								list_view->AddItem(new AttrItem( name, type, (bool)has_addon, (image_id)addon_id));
								list_view->Select(list_view->IndexOf(list_view->LastItem())); // keeps edit window happy
							}
			break;
		}
		case MAKE_NEW_ATTRIBUTE:
		{
			// to make a new attribute:
			// - add _id_ to a msg
			// - send the message to the AddAttrWindow
			BMessage command(MAKE_NEW_ATTRIBUTE);
			
			command.AddInt32("win id", _id_);
			add_attr_win->PostMessage(&command);
			break;
		}
		case ADD_NEW_ATTRIBUTE:
		{
			// what you should get after sending a MAKE_NEW_ATTRIBUTE msg
			// msg contains:
			// "attr name" - string
			// "addon id" - image_id
			// "type" - type_code
			// "has addon" - bool
			const char *name;
			int32 typecode;
			type_code type;
			ColumnListView *list_view = static_cast<ColumnListView *>(FindView("Attribute List"));
			
			if(list_view != NULL)
				if(msg->FindInt32("type", &typecode) == B_NO_ERROR)
						if(msg->FindString("attr name", &name) == B_NO_ERROR)
						{
							type = typecode;
							AddListItem(name, type, list_view);
						}
			break;
		}
		case SAVE_FILE:
		{
			//printf("AttrWindow::MessageReceived(). SAVE_FILE msg.\n");									
			BMessage *save_msg = new BMessage(SAVE_FILE);
			//save_msg->AddString("filename", Title());
			save_msg->AddInt32("win id", _id_); // new way
			//save_msg->PrintToStream();
			//save_container->PrintToStream();
			//printf("sending save_msg to be_app.\n");
			be_app->PostMessage(save_msg);
			//printf("deleteing save_msg.\n");
			delete save_msg;
			break;
		}
		case SAVE_AS_FILE:
		{
			// send the attributes off to be_app and let it deal
			// with the filepanel
			BMessage *save_msg = new BMessage(SAVE_AS_FILE);
			save_msg->AddInt32("win id", _id_); // new way
			//save_msg->PrintToStream();
			//attribute_container->PrintToStream();
			//printf("sending save_msg to be_app.\n");
			be_app->PostMessage(save_msg);
			//printf("deleteing save_msg.\n");
			delete save_msg;
			break;
		}
		case SAVE_BEFORE_QUITTING:
		{
			// msg contains:
			// "which" - int32 indicating which button was pressed on Save Alert.
			// 0 = don't save, 1 = cancel, 2 = OK
			int32 which;
			
			if(msg->FindInt32("which", &which) == B_NO_ERROR)
			{
				if(which == 0)
				{
					break;
				}
				if(which == 1)
				{	
					ok_to_quit = true;
					PostMessage(B_QUIT_REQUESTED);
					break;
				}
				// else ...
				BMessage *save_msg = new BMessage(SAVE_FILE);
			
				save_msg->AddInt32("win id", _id_);
				be_app->PostMessage(save_msg);
				delete save_msg;
				
				//PostMessage(B_QUIT_REQUESTED); // then quit
			}
			break;
		}
		case B_SIMPLE_DATA:
		{
			if(msg->WasDropped())
			{
				//msg->PrintToStream();
				if(msg->HasData("refs", B_ANY_TYPE))
				{
					msg->what = B_REFS_RECEIVED;
					be_app->PostMessage(msg);
				}
			}
			break;
		}
		default:
			BWindow::MessageReceived(msg);
			break;
	}
}

void AttrWindow::WindowActivated(bool active)
{
	if(active)
	{
		BMessage msg(WINDOW_ACTIVATED);
		msg.AddInt32("win id", _id_);
		edit_win->PostMessage(&msg);
	}
}

int32 AttrWindow::BuildMenubar(int32 width, int32 height)
{
	BRect		rect(0,0, width, height);
	BMenuBar	*menubar;
	BMenu		*menu, *edit_menu;
	const char	*string_ptr;
	
	menubar = new BMenuBar(rect, "menubar");
	
	// add a menu
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_FILE);
	menu = new BMenu(string_ptr);
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_NEW_FILE);
	menu->AddItem(new BMenuItem(string_ptr, new BMessage(NEW_FILE), 'N', B_COMMAND_KEY));
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_OPEN_FILE);
	menu->AddItem(new BMenuItem(string_ptr, new BMessage(OPEN_FILE), 'O', B_COMMAND_KEY));
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_CLOSE);
	menu->AddItem(new BMenuItem(string_ptr, new BMessage(B_QUIT_REQUESTED), 'W', B_COMMAND_KEY));
	menu->AddItem(new BSeparatorItem());
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_SAVE);
	menu->AddItem(new BMenuItem(string_ptr, new BMessage(SAVE_FILE), 'S', B_COMMAND_KEY));
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_SAVE_AS);
	menu->AddItem(new BMenuItem(string_ptr, new BMessage(SAVE_AS_FILE)));
	menu->AddItem(new BSeparatorItem());
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_ABOUT);
	menu->AddItem(new BMenuItem(string_ptr, new BMessage(B_ABOUT_REQUESTED)));
	menu->AddItem(new BSeparatorItem());
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_QUIT);
	menu->AddItem(new BMenuItem(string_ptr, new BMessage(B_QUIT_REQUESTED), 'Q', B_COMMAND_KEY));
	((BMenuItem *)menu->ItemAt(0))->SetTarget(be_app);
	((BMenuItem *)menu->ItemAt(1))->SetTarget(be_app);
	//((BMenuItem *)menu->ItemAt(5))->SetEnabled(false);
	((BMenuItem *)menu->ItemAt(7))->SetTarget(be_app);
	((BMenuItem *)menu->ItemAt(9))->SetTarget(be_app);
	
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_EDIT);
	edit_menu = new BMenu(string_ptr);
	
	// Add Attr
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_ADD_ATTR);
	edit_menu->AddItem(new BMenuItem(string_ptr, new BMessage(MAKE_NEW_ATTRIBUTE)));
	
	// Add Predefined Attr
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_ADD_PREDEF_ATTR);
	edit_menu->AddItem(new BMenu(string_ptr));
	
	// Remove Attr
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_REMOVE_ATTR);
	edit_menu->AddItem(new BMenuItem(string_ptr, new BMessage(REMOVE_ATTRIBUTE)));
	//((BMenuItem *)edit_menu->ItemAt(0))->SetEnabled(false);
	//((BMenuItem *)edit_menu->ItemAt(1))->SetEnabled(false);
	
	menubar->AddItem(menu);
	menubar->AddItem(edit_menu);
	
	SetKeyMenuBar(menubar);
	AddChild(menubar); // if we add it to the window, we can use it to size our view
	return((int32)(menubar->Bounds().Height() + 1));
}

void AttrWindow::Config(ColumnListView *list_view, BMessage *msg)
{
	// AttrItem(char* text0, char* text1, bool has_addon, image_id addon_id);
	//list_view->AddItem(new AttrItem("Attr Name","Type", true||false, addon_id));
	//msg->PrintToStream(); // debug info
	
	char *name_ptr;
	type_code type;
	int32 index, num_entries = msg->CountNames(B_ANY_TYPE);
	
	for(index=0; index <= num_entries; index++)
	{
		if(msg->GetInfo(B_ANY_TYPE, index, &name_ptr, &type) == B_NO_ERROR)
		{
			AddListItem(name_ptr, type, list_view);
			if((type == B_MIME_STRING_TYPE) && (strcmp(name_ptr, "BEOS:TYPE") == 0))
			{
				//printf("Found a \"BEOS:TYPE\" attribute\n");
				const char *type_name;
				ssize_t size;
				if(msg->FindData(name_ptr, type, (const void **)&type_name, &size) == B_NO_ERROR)
					FileTypeChanged(type_name);
			}
		}
	}
}

void AttrWindow::AddListItem(const char *name, type_code type, ColumnListView *list_view)
{
	//list_view->AddItem(new AttrItem(name,types[type_num],some_bool,id));
	// ptr = the name; id = the image_id of the addon; some_bool = is there an addon for this type?
	
	image_id id;
	int32 type_num=8;
	char *unknown_type;
	const char *string_ptr;
	
	if(list_view != NULL)
	{
		switch(type)
		{
			case B_STRING_TYPE:
			case B_MIME_STRING_TYPE:
			{
				type_num = 0;
				string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_STRING);
				break;
			}
			case B_INT32_TYPE:
			{
				type_num = 0;
				string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_INT32);
				break;
			}
			case B_INT64_TYPE:
			{
				type_num = 0;
				string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_INT64);
				break;
			}
			case B_BOOL_TYPE:
			{
				type_num = 0;
				string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_BOOL);
				break;
			}
			case B_DOUBLE_TYPE:
			{
				type_num = 0;
				string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_DOUBLE);
				break;
			}
			case B_FLOAT_TYPE:
			{
				type_num = 0;
				string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_FLOAT);
				break;
			}
			case B_RAW_TYPE:
			{
				type_num = 0;
				string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_RAW);
				break;
			}
			case B_RECT_TYPE:
			{
				type_num = 0;
				string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_RECT);
				break;
			}
			default:
			{
				type_num = 8;
				// do a quick conversion to ascii, it may or may not look good
				unknown_type = (char *)malloc(7);
				unknown_type[0] = '\'';
				unknown_type[1] = ((type>>24) & 255);
				unknown_type[2] = ((type>>16) & 255);
				unknown_type[3] = ((type>>8) & 255);
				unknown_type[4] = (type & 255);
				unknown_type[5] = '\'';
				unknown_type[6]= '\0';
				break;
			}
		}

		BMessage query(DO_YOU_SUPPORT_THIS_TYPE), reply;

		query.AddInt32("typecode", (int32)type);
		AddonMessenger->SendMessage(&query, &reply); // syncronous

		if(reply.what == ANSWER_YES)
		{
			reply.FindInt32("id", &id);
			if(type_num != 8)
				list_view->AddItem(new AttrItem(name,string_ptr,true, id));
			else
			{
				list_view->AddItem(new AttrItem(name,unknown_type,true, id));
				free(unknown_type);
				unknown_type = NULL;
			}
		}
		else
		{
			if(type_num != 8)
				list_view->AddItem(new AttrItem(name,string_ptr,false, -0x7f000000));
			else
			{
				list_view->AddItem(new AttrItem(name,unknown_type,false, -0x7f000000));
				free(unknown_type);
				unknown_type = NULL;
			}
		}
	}
}

void AttrWindow::RemoveListItem(const char *name, ColumnListView *list_view)
{
	//printf("removing list item: %s\n", name);
	AttrItem *item;
	int32 i=0;
	
	while((item = (AttrItem *)list_view->ItemAt(i++)) != NULL)
	{
		if(!strcmp(name, item->AttrName())) // found it
		{	
			//printf("found the list item.\n");
			((BListView *)list_view)->RemoveItem(i-1);
			//printf("removed it\n");
			delete item;
			//printf("deleted it\n");
		}
	}
}

void AttrWindow::FileTypeChanged(const char *new_type)
{
	//printf("FileTypeChanged() to %s\n", new_type);
	BMimeType a_type;
	BMessage attr_info, *menu_item_msg;
	BMenu *sub_menu;
	BMenuItem *remove_item;
	const char *attr_name, *pub_attr_name;
	char *initial_attr_data = (char *)malloc(1);
	int32 attr_type, index=0;
	
	//attr_info.PrintToStream();

	// second menu over, second item in that menu
	sub_menu = KeyMenuBar()->SubmenuAt(1)->SubmenuAt(1);
	
	// empty out the current menu
	while((remove_item = sub_menu->RemoveItem((int32)0)) != NULL)
		delete remove_item;
	
	if(a_type.SetTo(new_type) < B_NO_ERROR)
		return;
	a_type.GetAttrInfo(&attr_info);
	
	if(attr_info.IsEmpty())
		return;
	
	memset((void *)initial_attr_data,0,1);
	while(attr_info.FindString("attr:public_name", index, &pub_attr_name) == B_OK)
	{
		menu_item_msg = new BMessage(ADD_NEW_ATTRIBUTE);
		attr_info.FindString("attr:name", index, &attr_name);
		attr_info.FindInt32("attr:type", index, &attr_type);
		menu_item_msg->AddInt32("win id", _id_);
		menu_item_msg->AddString("attr name", attr_name);
		menu_item_msg->AddInt32("type", attr_type);
		menu_item_msg->AddData("attribute", attr_type, (void *)initial_attr_data, 1);
		sub_menu->AddItem(new BMenuItem(pub_attr_name, menu_item_msg));
		menu_item_msg = NULL;
		index++;
	}
	free(initial_attr_data);
	
	sub_menu->SetTargetForItems(*AttributeMessenger);
}

void AttrWindow::EditItem(const char *name, image_id addon_id)
{
	// Called from ColumnListView::SelectionChanged(). The window is locked.
	
	//printf("AttrWindow::EditItem()\n");
	BMessage msg(EDIT_ATTRIBUTE);
	
	msg.AddInt32("win id", _id_); // whether or not there is an addon, this must be in the msg
	
	if(addon_id == -1)
	{
		// the attribute selected is either NULL or has no addon
		msg.what = CLEAR_YOUR_VIEW;
		edit_win->PostMessage(&msg);
		
	}
	else
	{
		// it has an addon. Send it something meaningful
		msg.AddInt32("addon id", addon_id);
		msg.AddString("attr name", name);
		AttributeMessenger->SendMessage(&msg);
	}
}

void AttrWindow::Register(bool need_id)
{
	BMessenger messenger(APP_SIGNATURE);
	BMessage message1(WINDOW_REGISTRY_ADD),message2(NEW_WINDOW);
	
	message1.AddBool("need_id", need_id);
	messenger.SendMessage(&message1, this);
	
	// register with the editor window
	message2.AddInt32("win id", _id_);
	edit_win->PostMessage(&message2);
}

void AttrWindow::Unregister()
{
	BMessenger messenger(APP_SIGNATURE);
	BMessage msg(WINDOW_REGISTRY_SUB);
	msg.AddInt32("win id", _id_);
	// for be_app
	messenger.SendMessage(&msg);
	
	// for the attribute manager & the edit window
	msg.what = WINDOW_CLEANUP;
	AttributeMessenger->SendMessage(&msg);
	edit_win->PostMessage(&msg);
}
