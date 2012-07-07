/*
	
	EditorWindow.cpp
	John Wiggins 1998

*/

#include "EditorWindow.h"

EditorWindow::EditorWindow(BRect frame)
		:BWindow(frame,"Edit",B_TITLED_WINDOW_LOOK,B_FLOATING_SUBSET_WINDOW_FEEL,B_NOT_CLOSABLE|B_NOT_ZOOMABLE)
{
	BFont font(be_plain_font);
	int32 interfaceUnit = ((int32)font.Size());
	BRect rect = Bounds();
	const char *string_ptr;
	
	SetSizeLimits(200, 1<<20, 150, 1<<20); // set some realistic limits on size
	
	// create the list that keeps track of all the AttrWindows
	window_list = new BList(10);
	
	// make a background view for uniformity
	background = new BView(rect,"ATTRAPP:BackgroundView", B_FOLLOW_ALL, 0);
	
#ifndef B_BEOS_VERSION_4 /* R3 doesn't even define B_BEOS_VERSION ... */
	background->SetViewColor(216,216,216, 255);
#elif B_BEOS_VERSION >= B_BEOS_VERSION_4 /* BeOS R4 has the handy-dandy ui_color thingy */
	background->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
#endif

	AddChild(background);
	
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_OK);
	rect.Set(rect.right - (background->StringWidth(string_ptr) + (interfaceUnit<<2)), rect.bottom - (interfaceUnit*2.5), rect.right - interfaceUnit, rect.bottom - (interfaceUnit<<1));
	ok_button = new BButton(rect, "ok_button", string_ptr, new BMessage(APPLY_ATTRIBUTE_EDIT), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
	ok_button->SetEnabled(false); // bad things happen when no addon is present
	background->AddChild(ok_button);
	
	addon_frame_bottom = ((int32)ok_button->Bounds().top + (interfaceUnit<<1));
	
	// make a stub view so windows that don't have a current selection or have an uneditable attribute selected
	// can have a view to Hide()/Show() when the moment arises. Kinda kludgy, but it should keep us from
	// crashing without a whole bunch of extra work.
	rect.Set(0,0, frame.Width(), addon_frame_bottom);
	stub = new BView(rect, "ATTRAPP:StubView", B_FOLLOW_ALL, 0);
#ifndef B_BEOS_VERSION_4 /* R3 doesn't even define B_BEOS_VERSION ... */
	stub->SetViewColor(216,216,216, 255);
#elif B_BEOS_VERSION >= B_BEOS_VERSION_4 /* BeOS R4 has the handy-dandy ui_color thingy */
	stub->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
#endif
	background->AddChild(stub);
	
	current_view = stub;
	current_attr_type = 0x7fffffff;
	current_attr_name = NULL;
	// current_window_id is the id of the AttrWindow that last became the active window
	current_window_id = -1; // there are no AttrWindows when our ctor is called
	
	// set the title to "Edit" or non-english equivalent
	string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_EDIT);
	SetTitle(string_ptr);	
}

EditorWindow::~EditorWindow()
{
	void *list_item;
	for(int32 i=0;(list_item = window_list->ItemAt(i)) != NULL;i++)
	{
		((window_list_entry *)list_item)->view = NULL;
		delete list_item;
	}
	delete window_list;
}

bool EditorWindow::QuitRequested()
{
	// if we need to save, prompt before dying
	return BWindow::QuitRequested();
}

void EditorWindow::MessageReceived(BMessage *msg)
{
	//printf("EditorWindow::MessageReceived(). start.\n");
	//msg->PrintToStream();
	switch(msg->what)
	{
		case NEW_WINDOW:
		{
			// msg contains:
			// "win id" - unique id of new window - int32
			// action:
			// - add an entry to the window_list
			window_list_entry *list_item;
			int32 win_id;
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
			{
				//printf("Adding new window %ld\n", win_id);
				list_item = new window_list_entry;
				list_item->window_id = win_id;
				list_item->ok_button_state = false;
				list_item->view = stub;
				list_item->attr_name = NULL;
				list_item->attr_type = 0x7fffffff;
				window_list->AddItem((void *)list_item);
			}
			break;
		}
		case WINDOW_CLEANUP:
		{
			// msg contains:
			// "win id" - unique id of doomed window - int32
			// action;
			// remove the window_list_entry associated w/ "win id"
			int32 win_id;
			window_list_entry *list_item = NULL;
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
			{
				//printf("cleaning up window %d\n", win_id);
				if((list_item = GetListItem(win_id)) != NULL)
				{
					window_list->RemoveItem(list_item);
					if((list_item->view != stub) && (list_item->view != NULL))
					{
						list_item->view->RemoveSelf();
						delete list_item->view;
						list_item->view = NULL;
					}
					if(list_item->attr_name)
						delete [] list_item->attr_name;
					list_item->attr_name = NULL;
					delete list_item;
					
					// finish up 
					current_view = stub;
					current_attr_type = 0x7fffffff;
					current_attr_name = NULL;
					current_window_id = -1;
					ChangeWindowTitle();
					ok_button->SetEnabled(false);
					while(stub->IsHidden())
						stub->Show();
				}
			}
			break;
		}
		case WINDOW_ACTIVATED:
		{
			// msg contains:
			// "win id" - unique id of the window that just go a B_WINDOW_ACTIVATED msg - int32
			// action:
			// - if "win id" != current_window_id, make it == current_window_id
			//	then hide the current view and unhide "win id"->view
			int32 win_id;
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
				if(current_window_id != win_id)
					AttrWindowActivated(win_id);
			//printf("window %d activated\n", win_id);
			break;
		}
		case CLEAR_YOUR_VIEW:
		{
			// msg contains:
			// "win id" - unique window id of window that needs it's view cleared - int32
			// action:
			// - delete "win id"->view and replace it w/ stub
			// - delete "win id"->attr_name and set it to NULL
			// - set "win id"->attr_type to -1
			// - Disable ok button and set "win id"->ok_button_state to false
			// - call ChangeWindowTitle()
			int32 win_id;
			window_list_entry *list_item = NULL;
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
				if((list_item = GetListItem(win_id)) != NULL)
				{
					if(list_item->view != stub)
						if(background->RemoveChild(list_item->view))
						{
							delete list_item->view;
							list_item->view = stub;
						}
					list_item->ok_button_state = false;
					ok_button->SetEnabled(false); // disable before destroying current_attr_name
					while(stub->IsHidden())
						stub->Show();
					delete [] list_item->attr_name;
					list_item->attr_name = NULL;
					list_item->attr_type = 0x7fffffff;
				}
			
			current_view = stub;
			current_attr_type = 0x7fffffff;
			current_attr_name = NULL;
			ChangeWindowTitle(); // make the window title reflect the current attribute ( or lack thereof )
			break;
		}
		case APPLY_ATTRIBUTE_EDIT:
		{
			// the ok button was pressed
			window_list_entry *list_item = NULL;
			
			if((list_item = GetListItem(current_window_id)) != NULL)
				if(list_item->view != stub)
					SendAttributeToParent((AttrAddon *)list_item->view);
			break;
		}
		case EDIT_ATTRIBUTE:
		{
			// got a message from the main window indicating that a different attribute
			// has been selected. The window will only send us this msg if the attribute
			// is supported. (has an add-on)
			//printf("EditorWindow::MessageReceived(). got something to edit.\n");
			SwapEditView(msg);
			break;
		}
		case ATTR_MANAGER_GREET:
		{
			// msg contains:
			// "me" - a pointer to the_attribute_manager - BLooper(s and practical jokes (har har))
			// action:
			// - assign "me" to a local pointer
			BLooper *ptr;
			
			if(msg->FindPointer("me", (void **)&ptr) == B_NO_ERROR)
				attribute_manager = ptr;
			break;
		}
		default:
			BWindow::MessageReceived(msg);
			break;
	}
	//printf("EditorWindow::MessageReceived(). end.\n");
}

void EditorWindow::SwapEditView(BMessage *msg)
{
	// msg contains:
	// "win id" - unique id of the window that ownz (:P) this attribute - int32 (remove)
	// "addon id" - id of addon that can handle attribute - image_id (remove)
	// "attr name" - the attribute's name - string (remove)
	// "typecode" - type_code of the attribute - int32
	// "attribute" - ignored. the addon cares about this
	// "size" - ignored. the addon needs to care about this.
	// action:
	// - too complicated to explain.
	// - "change out views" :P
	image_id addon_id;
	int32 typecode, win_id;
	int32 namelen;
	status_t ret = B_ERROR; // the return value of the instantiation func
	const char *name_ptr;
	window_list_entry *list_item = NULL;
	attr_addon_instantiation_func instantiate; // typedef'd as a double pointer
	AttrAddon *addonview;
	BRect bounds_rect = Bounds();
	bounds_rect.InsetBy(5,5);
	bounds_rect.bottom -= addon_frame_bottom;
	
	// sanity check
	if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
	{
		if(win_id != current_window_id)
			AttrWindowActivated(win_id);
		// proceed as planned
		if(msg->FindInt32("addon id", &addon_id) == B_NO_ERROR)
		{
			if(msg->FindInt32("typecode", &typecode) == B_NO_ERROR)
			{
				if(msg->FindString("attr name", &name_ptr) == B_NO_ERROR)
				{
					if((list_item = GetListItem(win_id)) != NULL)
					{
						//printf("GetListItem(win_id) wasn't NULL!\n");
						// disable the ok button
						ok_button->SetEnabled(false);
					
						// whole buncha jaz with current_attr_name and friends
						// delete current_attr_name
						delete [] list_item->attr_name;
						list_item->attr_name = NULL;
						current_attr_name = NULL;
						// copy name_ptr into list_item->attr_name
						namelen = strlen(name_ptr);
						list_item->attr_name = new char [ namelen + 1];
						memset((void *)list_item->attr_name, 0, namelen);
						strcpy(list_item->attr_name, name_ptr);
						list_item->attr_type = typecode;
						
						current_attr_name = list_item->attr_name;
						current_attr_type = list_item->attr_type;
						
						if(get_image_symbol(addon_id, "Instantiate", B_SYMBOL_TYPE_TEXT, (void **)&instantiate) == B_NO_ERROR)
						{
							//printf("EditorWindow::MessageReceived(). grabbed a pointer to Instantiate()\n");
							// okie dokie. instatiate a view with our newfound function
							// first clear some data from the msg that the addon won't need 
							// (and shouldn't know about >:)
							msg->RemoveName("win id");
							msg->RemoveName("addon id"); // the image_id of the add-on
							msg->RemoveName("attr name"); // the name of the attribute. 
												// might actually send this to the add-on if the type is B_RAW_TYPE
						
							// instantiate
							//printf("EditorWindow::MessageReceived(). instantiating\n");
							addonview = instantiate(bounds_rect, "Addon View", B_FOLLOW_ALL, B_WILL_DRAW, msg, &ret);
							if(ret == B_NO_ERROR)
							{
								while(current_view->IsHidden())
									current_view->Show(); // make sure it's showing before we hide it
								current_view->Hide();
								//printf("EditorWindow::MessageReceived(). instantiated successfully!\n");
								if(list_item->view != stub)
								{
									//printf("EditorWindow::MessageReceived(). list_item->view != stub\n");
									//printf("EditorWindow::MessageReceived(). removing old addon view\n");
									if(background->RemoveChild(list_item->view))
									{
										//if(remove->IsDirty())
										//	SendAttributeToParent((AttrAddon *)remove); // save the data
										delete list_item->view;
										list_item->view = NULL;
										//printf("EditorWindow::MessageReceived(). adding new addon view\n");
										if(addonview != NULL)
										{
											list_item->view = addonview;
											current_view = list_item->view;
											background->AddChild(addonview);
											list_item->ok_button_state = true;
											// - break -
										}
										else // backup in case of screwy addon
										{
											delete [] list_item->attr_name;
											list_item->attr_name = NULL;
											list_item->view = stub;
											current_view = stub;
											while(current_view->IsHidden())
												current_view->Show();
											list_item->attr_type = 0x7fffffff;
											list_item->ok_button_state = false;
											current_attr_name = NULL;
											current_attr_type = 0x7fffffff;
										}
									}
								}
								else /* list_item->view == stub */
								{
									if(addonview != NULL)
									{
										while(stub->IsHidden())
											stub->Show();
										stub->Hide();
										list_item->view = addonview;
										current_view = list_item->view;
										background->AddChild(addonview);
										list_item->ok_button_state = true;
									}
									else // backup in case of screwy addon
									{
										delete [] list_item->attr_name;
										list_item->attr_name = NULL;
										list_item->view = stub;
										current_view = stub;
										while(current_view->IsHidden())
											current_view->Show();
										list_item->attr_type = 0x7fffffff;
										list_item->ok_button_state = false;
										current_attr_name = NULL;
										current_attr_type = 0x7fffffff;
									}
								}
							} // ret == B_NO_ERROR
							else // ret != B_NO_ERROR. the addonview is invalid in some way
							{
								// the addon failed to instantiate
								delete [] list_item->attr_name;
								list_item->attr_name = NULL;
								list_item->view = stub;
								current_view = stub;
								while(current_view->IsHidden())
									current_view->Show();
								list_item->attr_type = 0x7fffffff;
								list_item->ok_button_state = false;
								current_attr_name = NULL;
								current_attr_type = 0x7fffffff;
							}
							// put this all in on place after the chaos above for code tidyness (heh)
							ChangeWindowTitle();
							ok_button->SetEnabled(list_item->ok_button_state); // enable only when title is current
						}
					}
					else
					{
						//printf("GetListItem() returned a NULL ptr\n");
						ok_button->SetEnabled(false);
						current_view = stub;
						while(current_view->IsHidden())
							current_view->Show();
						current_attr_name = NULL;
						current_attr_type = 0x7fffffff;
						ChangeWindowTitle();
					}
					// end of actual guts
				}
			}
		}
	}
}

void EditorWindow::SendAttributeToParent(AttrAddon *view)
{
	//printf("EditorWindow::SendAttributeToParent()\n");
	// grab data from remove and send in to _parent before destroying it
	BMessage attribute;
	
	if(view->GetData(&attribute) == B_NO_ERROR)
	{
		//printf("view->GetData() successful.\n");
		// got the data from the addon
		// stuff it into a msg and send it off to _parent
		attribute.AddString("attr name", current_attr_name);
		attribute.AddInt32("win id", current_window_id);
		attribute.what = REPLACE_ATTRIBUTE;
		//attribute.AddData(Title(), current_attr_type, data, size);
		attribute_manager->PostMessage(&attribute);
	}
}

void EditorWindow::AttrWindowActivated(int32 window_id)
{
	//printf("Window %ld activated\n", window_id);
	window_list_entry *list_item_cur = NULL, *list_item_new = NULL;
	
	list_item_new = GetListItem(window_id);
	list_item_cur = GetListItem(current_window_id);
	
	if((list_item_new != NULL) && (list_item_cur != NULL)) /* both windows got all da right info set up */
	{
		//printf("EditorWindow::AttrWindowActivated(). both windows are known\n");
		// woohoo. now do something.
		list_item_cur->ok_button_state = ok_button->IsEnabled(); // save the state
		ok_button->SetEnabled(false); // disable during chaos
		while(list_item_cur->view->IsHidden()) // make absolutely sure it's showing before we hide it
			list_item_cur->view->Show();
		list_item_cur->view->Hide(); // hide the current addon view
		
		while(list_item_new->view->IsHidden())
			list_item_new->view->Show(); // show the new addon view
		ok_button->SetEnabled(list_item_new->ok_button_state); // set the new state
		current_view = list_item_new->view;
		// set the current_* vars
		current_attr_name = list_item_new->attr_name;
		current_attr_type = list_item_new->attr_type;
		ChangeWindowTitle(); // make the window title reflect the current attribute
	}
	else /* one of the windows has no list item associated with it (probably a race condition happening at construction) */
	{
		//printf("EditorWindow::AttrWindowActivated(). one or both of the windows had no info on it.\n");
		if(list_item_cur != NULL) // make sure we have info for the previous current window (we should)
		{
			//printf("EditorWindow::AttrWindowActivated(). the current window had info available\n");
			list_item_cur->ok_button_state = ok_button->IsEnabled(); // save the state
			ok_button->SetEnabled(false); // we don't want no one pressing this
			while(list_item_cur->view->IsHidden()) // make absolutely sure it's showing before we hide it
				list_item_cur->view->Show();
			list_item_cur->view->Hide(); // hide the view
			
			current_view = stub;
			while(current_view->IsHidden())
				current_view->Show(); // show the new view (stub in this case)
			current_attr_name = NULL;
			current_attr_type = 0x7fffffff;
			ChangeWindowTitle(); // make the window title reflect the current attribute
		}
		else // current_window_id points to a just dead window (probably). set up the new window's stuff
		{
			//printf("EditorWindow::AttrWindowActivated(). the current window had no info on it\n");
			if(list_item_new != NULL)
			{
				ok_button->SetEnabled(false);
				while(list_item_new->view->IsHidden())
					list_item_new->view->Show(); // show the new addon view
				ok_button->SetEnabled(list_item_new->ok_button_state); // set the new state
				current_view = list_item_new->view;
				// set the current_* vars
				current_attr_name = list_item_new->attr_name;
				current_attr_type = list_item_new->attr_type;
				ChangeWindowTitle(); // make the window title reflect the current attribute
			}
		}
	}
		
	// and finally
	current_window_id = window_id;
}

void EditorWindow::ChangeWindowTitle()
{
	const char *string_ptr = ((AttrApp *)be_app)->res_strings->String(STRING_EDIT);
	// set the title of the window	
	if(current_attr_name != NULL)
	{
		char *NewTitle;
		NewTitle = (char *)malloc( strlen(string_ptr) + B_ATTR_NAME_LENGTH + 6 ); // 6 = " : " + quotes + \0
		sprintf(NewTitle, "%s : \"%s\"", string_ptr, current_attr_name);
		SetTitle(NewTitle);
		free(NewTitle);
		NewTitle = NULL;
	}
	else // current_attr_name == NULL
		SetTitle(string_ptr);
}

window_list_entry *EditorWindow::GetListItem(int32 window_id)
{
	window_list_entry *list_item;
	int32 i=0;
	
	while((list_item = (window_list_entry *)window_list->ItemAt(i++)) != NULL)
		if(list_item->window_id == window_id)
			return list_item;
	
	return (window_list_entry *)NULL;
}