/*

	AttributeManager.cpp - the brains of the operation
	John Wiggins 1998

*/

#include "AttributeManager.h"

AttributeManager::AttributeManager(EditorWindow *window)
:BLooper("Attribute Manager", B_NORMAL_PRIORITY, B_LOOPER_PORT_DEFAULT_CAPACITY)
{
	BMessenger messenger(window);
	BMessage greeting(ATTR_MANAGER_GREET);
	greeting.AddPointer("me", (void *)this);
	//if(messenger.IsValid())
	// fuck validity. if it isn't valid, we _want_ to crash. it'll be easier to catch that way
	messenger.SendMessage(&greeting);
	editor_window = window;
	file_list = new BList(10);
}

AttributeManager::~AttributeManager()
{
	void *list_item;
	for(int32 i=0;(list_item = file_list->ItemAt(i)) != NULL;i++)
	{
		delete [] ((file_list_entry *)list_item)->filename;
		delete ((file_list_entry *)list_item)->attributes;
		if(((file_list_entry *)list_item)->remove_list != NULL)
			delete ((file_list_entry *)list_item)->remove_list;
		delete list_item;
	}
	delete file_list;
}

void AttributeManager::MessageReceived(BMessage *msg)
{
	//msg->PrintToStream();
	switch(msg->what)
	{
		case IS_FILE_OPEN:
		{
			// msg contains:
			// "path" - path of the file - string
			// action:
			// - check to see if we already have file indicated by "path" open
			const char *path;
			
			if(msg->FindString("path", &path) == B_NO_ERROR)
			{
				// ok, now check the list
				if(IsFileOpen(path))
					msg->SendReply(ANSWER_YES);
				else
					msg->SendReply(ANSWER_NO);
			}
			else
				msg->SendReply(ANSWER_NO); // lie. quiet failure makes bugs more fun to find
			
			break;
		}
		case ADD_FILE_TO_LIST:
		{
			// msg contains:
			// "path" - path of the file - string
			// "win id" - unique window id of window handling file <path> - int32
			// "msg ptr" - a pointer to the BMessage holding file <path>'s attributes - void **
			// action:
			// - build a file_list_entry with the message's data
			// - at that file_list_entry to file_list
			const char *path;
			BMessage *container_ptr;
			int32 win_id;
			
			if(msg->FindString("path", &path) == B_NO_ERROR)
				if(msg->FindPointer("msg ptr", (void **)&container_ptr) == B_NO_ERROR)
					if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
						AddFileToList(path, container_ptr, win_id);
			break;
		}
		case GET_PATH_FOR_FILE:
		{
			// msg contains:
			// "win id" - unique window id of a window - int32
			// action:
			// - find file_list_entry associated with "win id" and return file_list_entry->filename
			int32 win_id;
			char *pathname = NULL;
			BMessage reply;
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
			{
				
				GetPathForFile(win_id, &pathname);
				if(pathname != NULL)
					reply.AddString("pathname", pathname);
				else
					reply.AddString("pathname", B_EMPTY_STRING);
			}
			else
				reply.AddString("pathname", B_EMPTY_STRING);
			//printf("GET_PATH_FOR_FILE: got path : %s\n", pathname);
			//reply.PrintToStream();
			msg->SendReply(&reply);
			break;
		}
		case SET_PATH_FOR_FILE:
		{
			// msg contains:
			// "win id" - unique window id of a window - int32 
			// "pathname" - pathname of a file - string
			// action:
			// - find file_list_entry associated with "win id"
			// - set file_list_entry->filename to "path"
			// - send a message to window with id "win id" that contains the leaf of the pathname
			int32 win_id;
			const char *pathname;
			BPath path;
			BMessage set_title_msg(WINDOW_SET_TITLE);
			BWindow *window;
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
				if(msg->FindString("pathname", &pathname) == B_NO_ERROR)
				{
					//printf("SetPathForFile(win_id = %d, pathname = %s);\n", win_id, pathname);
					SetPathForFile(win_id, pathname);
					path.SetTo(pathname);
					if(path.InitCheck() == B_NO_ERROR)
					{
						set_title_msg.AddString("title", path.Leaf());
						if((window = FindWindowByID(win_id)) != NULL)
							window->PostMessage(&set_title_msg);
					}
				}
			break;
		}
		case EDIT_ATTRIBUTE:
		{
			// msg contains:
			// "win id" - window's unique id - int32
			// "attr name" - name of the attribute - string
			// "addon id" - image_id of the addon that can handle this attribute - int32
			// action:
			// - find attribute indicated by "win id" and "attr name"
			// - add field "data" which is a pointer to the data of the attribute
			// - add feild "typecode" which is the type_code of the attribute
			// - add feild "datasize" which is the size (surprise!) of the attribute data
			// - send to editor_window
			int32 win_id;
			const char *attr_name;
			const void *attribute;
			ssize_t size;
			type_code type;
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
				if(msg->FindString("attr name", &attr_name) == B_NO_ERROR)
				{
					GrabAttribute(attr_name, win_id, &attribute, &size, &type);
					if((attribute != NULL) && (size != 0))
					{
						msg->AddInt32("datasize", size);
						msg->AddInt32("typecode", type);
						msg->AddPointer("data", attribute);
						editor_window->PostMessage(msg);
					}
				}
			break;
		}
		case REPLACE_ATTRIBUTE:
		{
			// msg contains:
			// "win id" - window that owns the attribute - int32
			// "attr name" - name of the attribute you need to replace - string
			// "data" - the attribute - typed data
			// action:
			// - replace attribute denoted by "attr name" and belonging to "win id" with
			// data in "attribute"
			int32 win_id;
			const char *attr_name;
			const void *attribute;
			type_code type;
			ssize_t size;
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
				if(msg->FindString("attr name", &attr_name) == B_NO_ERROR)
					if(msg->GetInfo("data", &type) == B_NO_ERROR)
						if(msg->FindData("data", type, &attribute, &size) == B_NO_ERROR)
						{
							ReplaceAttribute(attr_name, attribute, type, size, win_id); // do the normal action
							
							// check to see if the attribute is the BEOS:TYPE attribute
							if((type == B_MIME_STRING_TYPE) && (strcmp(attr_name, "BEOS:TYPE") == 0))
							{
								BWindow *window = FindWindowByID(win_id);
								BMessage type_change_msg(BEOS_TYPE_CHANGE);
								type_change_msg.AddString("new type", (const char *)attribute);
								if(window != NULL)
									window->PostMessage(&type_change_msg);
							}
						}
						
			break;
		}
		case DUPLICATE_ATTRIBUTE:
		{
			// msg contains:
			// "src win id" - window where the attribute is - int32
			// "dest win id" - window where the attribute needs to go - int32
			// "attr name" -  name of the attribute - string
			// action :
			// - pull data out of msg
			// - copy attribute <attr name> from list item associated w/ "src win id" and copy it into
			// 	list item associated w/ "dest win id"
			int32 src_win_id, dest_win_id, alert_result;
			const char *attr_name;
			status_t err = B_ERROR;
			bool overwrite = false;
			
			// check for msg repost that will occur whenever an alert is popped up
			if(msg->FindInt32("which", &alert_result) == B_NO_ERROR)
			{
				if(alert_result == 1)
				{
					msg->ReplaceBool("overwrite", true);
				}
				else
					return; // if they don't want to overwrite the attribute, then we have no business being here.
			}
			
			if(msg->FindInt32("src win id", &src_win_id) == B_NO_ERROR)
				if(msg->FindInt32("dest win id", &dest_win_id) == B_NO_ERROR)
					if(msg->FindString("attr name", &attr_name) == B_NO_ERROR)
						if(msg->FindBool("overwrite", &overwrite) == B_NO_ERROR)
							err = CopyAttrWinToWin(src_win_id, dest_win_id, attr_name, overwrite);
			if(err == B_ERROR)
				break; // we want to bail if something bad happened
			if(err == ERR_ALREADY_EXISTS)
			{
				// prompt to see if the user wants to replace
				const char *string_ptr1, *string_ptr2, *string_ptr3;
				BAlert *alert;
				BMessage *invoker_msg;
				
				string_ptr1 = ((AttrApp *)be_app)->res_strings->String(STRING_DUPLICATE_ATTR_WARNING);
				string_ptr2 = ((AttrApp *)be_app)->res_strings->String(STRING_CANCEL);
				string_ptr3 = ((AttrApp *)be_app)->res_strings->String(STRING_OK);
				
				alert = new BAlert("alert",string_ptr1,string_ptr2,string_ptr3, NULL, B_WIDTH_AS_USUAL, B_WARNING_ALERT);
				alert->SetShortcut(0,B_ESCAPE);
				
				invoker_msg = new BMessage(msg); // we need to make a copy. Bad Things® will happen otherwise.
				// the invoker owns this message. we don't have to delete it.
				
				alert->Go(new BInvoker(invoker_msg, this));
				invoker_msg = NULL;
			}
			else
			{
				// success. Tell the window that got the attribute, that it has a new attribute that it
				// needs to worry about.
				BMessage *reply = new BMessage(msg);
				BWindow *window = FindWindowByID(dest_win_id);
			
				reply->what = DRAGGED_ATTRIBUTE_LISTADD; // was an attribute replaced in this drag session?
				if(window != NULL)
					window->PostMessage(reply);
				delete reply;
			}
			break;
		}
		case REMOVE_ATTRIBUTE:
		{
			// msg contains:
			// "win id" - window id that wants action taken - int32
			// "attr name" - name of doomed attribute - string
			// action:
			// - remove the attribute indicated by "attr name" and associated w/ "win id"
			int32 win_id;
			const char *attr_name;
			file_list_entry *list_item = NULL;
			
			//msg->PrintToStream();
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
				if(msg->FindString("attr name", &attr_name) == B_NO_ERROR)
					if((list_item = GetListItem(win_id)) != NULL)
					{
						//list_item->attributes->PrintToStream();
						list_item->attributes->RemoveName(attr_name);
						if(list_item->remove_list == NULL)
							list_item->remove_list = new BMessage();
						if(!list_item->remove_list->HasString(attr_name)) // it's not already on the remove_list
							list_item->remove_list->AddString(attr_name, attr_name); // add it
						//list_item->attributes->PrintToStream();
						
						list_item->file_is_dirty = true;
					}
			break;
		}
		case ADD_NEW_ATTRIBUTE:
		{
			// msg contains:
			// "attr name" - name of newborn attribute - string
			// "attribute" - the attribute - raw data
			// "win id" - the id of the window owning this attribute - int32
			// attribute info : (ignore) "type"
			// action:
			// - add attribute from msg to data struct associated w/ "win id"
			// - if that is successful, remove "win id" & "attribute" and forward it to window w/ "win id"
			status_t err = B_ERROR;
			int32 win_id;
			BWindow *window = NULL;
			const char *attr_name;
			const void *attribute;
			type_code type; // attribute's type
			ssize_t size; // attribute's size
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
				if(msg->FindString("attr name", &attr_name) == B_NO_ERROR)
					if(msg->GetInfo("attribute", &type) == B_NO_ERROR)
						if(msg->FindData("attribute", type, &attribute, &size) == B_NO_ERROR)
							err = AddNewAttrToWin(attr_name, attribute, type, size, win_id);
			if(err == B_NO_ERROR)
				if((window = ((AttrApp *)be_app)->FindWindowByID(win_id)) != NULL)
				{
					msg->what = ADD_NEW_ATTRIBUTE;
					msg->RemoveName("win id");
					msg->RemoveName("attribute");
					window->PostMessage(msg);
				}
			
			break;
		}
		case GET_ATTRIBUTES:
		{
			// msg contains:
			// "win id" - unique id of window for which I have data - int32
			// action:
			// - get data associated w/ "win id"
			// - data refers to a BMessage. Send a pointer named "attrmsg" as a reply
			int32 win_id;
			file_list_entry *list_item = NULL;
			BMessage reply;
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
				if((list_item = GetListItem(win_id)) != NULL)
				{
					reply.AddPointer("attrmsg", (void *)list_item->attributes);
					if(list_item->remove_list != NULL)
						reply.AddPointer("removelist", (void *)list_item->remove_list);
					
					list_item->file_is_dirty = false; // this is safe. we only get this msg when the file is saved
				}
			msg->SendReply(&reply); // send the reply, empty or not. AttrApp will do the Right Thing ™
			break;
		}
		case WINDOW_CLEANUP:
		{
			// msg contains:
			// "win id" - id of dying window - int32
			// action:
			// - remove file_list_entry associated w/ "win id"
			int32 win_id;
			file_list_entry *list_item = NULL;
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
				if((list_item = GetListItem(win_id)) != NULL)
					if(file_list->RemoveItem((void *)list_item))
					{
						delete [] list_item->filename;
						delete list_item->attributes;
						if(list_item->remove_list != NULL)
							delete list_item->remove_list;
						delete list_item;
					}
			break;
		}
		case IS_FILE_DIRTY:
		{
			// msg contains:
			// "win id" - id of querying window - int32
			// action:
			// - find out if the window's file needs saving and respond accordingly
			int32 win_id;
			file_list_entry *list_item;
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
				if((list_item = GetListItem(win_id)) != NULL)
				{
					if(list_item->file_is_dirty)
						msg->SendReply(ANSWER_YES);
					else
						msg->SendReply(ANSWER_NO);
				}
			break;
		}
		default:
			BLooper::MessageReceived(msg);
			break;
	}
}

bool AttributeManager::IsFileOpen(const char *path)
{
	int32 i=0;
	bool found = false;
	file_list_entry *list_item;
	
	// sequential search. sue me. :P
	while(!found && ((list_item = (file_list_entry *)file_list->ItemAt(i++)) != NULL))
		if(strcmp(path, list_item->filename) == 0)
			found = true;
	
	return found;
}

void AttributeManager::AddFileToList(const char *path, BMessage *container, int32 window_id)
{
	file_list_entry *new_entry;
	
	new_entry = new file_list_entry;
	new_entry->window_id = window_id;
	new_entry->filename = new char [ strlen(path) + 1 ];
	strcpy(new_entry->filename, path);
	new_entry->attributes = container;
	new_entry->remove_list = NULL;
	new_entry->file_is_dirty = false;
	
	file_list->AddItem((void *)new_entry);
}

void AttributeManager::GrabAttribute(const char *name, int32 window_id, const void **attribute, ssize_t *size, type_code *type)
{
	file_list_entry *list_item = NULL;
	
	if((list_item = GetListItem(window_id)) != NULL)
		if(list_item->attributes->GetInfo(name, type) == B_NO_ERROR)
			if(list_item->attributes->FindData(name, B_ANY_TYPE, attribute, size) == B_NO_ERROR)
				return;

	// make note of your failure
	*size = 0;
	*attribute = NULL;
}

void AttributeManager::ReplaceAttribute(const char *name, const void *attribute, type_code type, ssize_t size, int32 window_id)
{
	file_list_entry *list_item = NULL;
	
	if((list_item = GetListItem(window_id)) != NULL)
	{
		list_item->attributes->ReplaceData(name, type, attribute, size);
		list_item->file_is_dirty = true;
	}
		
}

void AttributeManager::GetPathForFile(int32 window_id, char **pathname)
{
	file_list_entry *list_item = NULL;
	
	if((list_item = GetListItem(window_id)) != NULL)
	{
		*pathname = list_item->filename;
		//printf("GetPathForFile(). pathname = %s, list_item->filename = %s\n", pathname, list_item->filename);
	}
	else
	{
		//printf("there is no list item for that window_id.\n");
		*pathname = NULL;
	}
}

void AttributeManager::SetPathForFile(int32 window_id, const char *pathname)
{
	file_list_entry *list_item = NULL;
	
	if((list_item = GetListItem(window_id)) != NULL)
	{
		delete [] list_item->filename;
		list_item->filename = new char [strlen(pathname) + 1];
		strcpy(list_item->filename, pathname);
		//printf("Path set to : %s\n", list_item->filename);
	}
	//else
	//	printf("there aint no list item for that window id!\n");
}

status_t AttributeManager::CopyAttrWinToWin(int32 from_win, int32 to_win, const char *name, bool overwrite)
{
	file_list_entry *list_item = NULL;
	status_t err = B_ERROR;
	type_code typecode;
	const void *src_data;
	ssize_t data_size;
	
	if((list_item = GetListItem(from_win)) != NULL)
	{
		// found the source window data struct
		// get the type_code of the attr first
		if(list_item->attributes->GetInfo(name, &typecode) == B_NO_ERROR)
			if(list_item->attributes->FindData(name, typecode, &src_data, &data_size) == B_NO_ERROR)
			{
				// look for the dest window data struct
				if((list_item = GetListItem(to_win)) != NULL)
				{
					// man we're getting nested :)
					if(list_item->attributes->HasData(name, B_ANY_TYPE) && !overwrite)
						return ERR_ALREADY_EXISTS;
					if(overwrite)
						list_item->attributes->RemoveName(name); // make sure you trash the old one 
					err = list_item->attributes->AddData(name, typecode, src_data, data_size, false);
					list_item->file_is_dirty = true;
					// check to see if attribute with "name" was previously removed.
					// if so, take it off the remove list
					if(list_item->remove_list != NULL)
						if(list_item->remove_list->HasString(name))
							list_item->remove_list->RemoveName(name);
					return err;
				}
			}
	}
	return err;
}

status_t AttributeManager::AddNewAttrToWin(const char *name, const void *data, type_code type, ssize_t size, int32 window_id)
{
	file_list_entry *list_item = NULL;
	status_t err = B_ERROR;
	
	if((list_item = GetListItem(window_id)) != NULL)
	{
		if(!list_item->attributes->HasData(name, B_ANY_TYPE)) // doesn't already exist
			err = list_item->attributes->AddData(name, type, data, size, false);
		else
		{
			err = ERR_ALREADY_EXISTS;
			return err;
		}
		// check to see if attribute with "name" was previously removed.
		 // if so, take it off the remove list
		if(list_item->remove_list != NULL)
			if(list_item->remove_list->HasString(name))
				list_item->remove_list->RemoveName(name);
		
		list_item->file_is_dirty = true;
	}
	
	return err;
}

file_list_entry *AttributeManager::GetListItem(int32 window_id)
{
	file_list_entry *list_item;
	int32 i=0;
	
	while((list_item = (file_list_entry *)file_list->ItemAt(i++)) != NULL)
		if(list_item->window_id == window_id)
			return list_item;
	
	return (file_list_entry *)NULL;
}

BWindow *AttributeManager::FindWindowByID(int32 window_id)
{
	BWindow *window = NULL;
	AttrWindow *attrwin = NULL;
	int32 i=0;
	
	while((window = be_app->WindowAt(i++)) != NULL)
		if(window->Lock())
		{
			// do a cast so we don't call ID() for some window that doesn't have it (bad thing ™)
			if((attrwin = static_cast<AttrWindow *>(window)) != NULL)
				if(attrwin->ID() == window_id)
				{
					attrwin->Unlock();
					return attrwin;
				}
			window->Unlock();
		}
	
	return window;
}