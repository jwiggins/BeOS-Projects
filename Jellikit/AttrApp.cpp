/*
	
	AttrApp.cpp - an extensible attribute editor
	John Wiggins 1998
	
*/

#include "AttrApp.h"

int main(void)
{	
	AttrApp myApplication;
	myApplication.Run();
	return B_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//  AttrApp ctor
///////////////////

AttrApp::AttrApp()
		: BApplication(APP_SIGNATURE), window_count(0), window_id_source(0), next_untitled_number(1), about_box_alive(false)
{
	status_t err;
	const char *string_ptr1, *string_ptr2, *string_ptr3;
	char *allocd_string;
	// construct the AppResStrings object so we can get our displayable strings
	res_strings = new AppResStrings();
	// construct an AddonManager™ for handy addon management
	the_addon_manager = new AddonManager();
	err = the_addon_manager->Run();
	if(err < B_NO_ERROR)
	{
		//printf("error\n");
		string_ptr1 = res_strings->String(STRING_NO_MANAGER_WARNING);
		//printf("got \"no manager\" string. %s\n", string_ptr1);
		string_ptr2 = "add-on";
		//printf("got \"add-on\" string. %s\n", string_ptr2);
		string_ptr3 = res_strings->String(STRING_OK);
		//printf("got \"ok\" string. %s\n", string_ptr3);
		allocd_string = (char *)malloc(strlen(string_ptr1) + strlen(string_ptr2) + 1);
		//printf("allocated error_string\n");
		sprintf(allocd_string, string_ptr1, string_ptr2);
		BAlert *alert = new BAlert("start failure", allocd_string, string_ptr3);
		alert->Go();
		PostMessage(B_QUIT_REQUESTED); // safer than Quit() ...
		free(allocd_string);
		return;
	}
	else
	{
		//////////////////////////////////////////////////////
		// INSTANTIATE THE EDITOR WINDOW FIRST!!! //
		//////////////////////////////////////////////////////
		BRect rect(435, 50, 634, 199);
		the_editor_window = new EditorWindow(rect);
		// construct an AttributeManager™ for handy addon management
		the_attr_manager = new AttributeManager(the_editor_window);
		err = the_attr_manager->Run();
		if(err < B_NO_ERROR)
		{
			string_ptr1 = res_strings->String(STRING_NO_MANAGER_WARNING);
			string_ptr2 = "attribute";
			string_ptr3 = res_strings->String(STRING_OK);
			allocd_string = (char *)malloc(strlen(string_ptr1) + strlen(string_ptr2) + 1);
			sprintf(allocd_string, string_ptr1, string_ptr2);
			BAlert *alert = new BAlert("start failure", allocd_string, string_ptr3);
			alert->Go();
			PostMessage(B_QUIT_REQUESTED); // safer than Quit() ...
			free(allocd_string);
			return;
		}
		else
		{
			// construct the "Add Attribute" window
			rect.Set(150,100, 399,219);
			string_ptr1 = res_strings->String(STRING_ADD_ATTR);
			the_addattr_window = new AddAttrWindow(rect, string_ptr1, the_addon_manager, the_attr_manager);
			// construct the AttrMessenger
			AttrMessenger = new BMessenger(the_attr_manager);
		}
	}
	
	BMediaRoster::Roster(&err); // err will tell all!
	if(err < B_NO_ERROR)
		media_server_is_alive = false;
	else
		media_server_is_alive = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//  QuitRequested
///////////////////

bool AttrApp::QuitRequested()
{
	BWindow *window; 
	int32 i = 0;
	
	// delete the filepanel(s)
	delete OpenPanel;
	delete SaveAsPanel;
	
	// tell all the windows to die
	while((window = WindowAt(i++)) != NULL)
	{
		//window->Quit();
		window->PostMessage(B_QUIT_REQUESTED);
	}
	//  wait for them to die
	while(CountWindows() > 0)
		snooze(50 * 1000);
	
	if(AttrMessenger)
		delete AttrMessenger;
	if(the_addon_manager)
		the_addon_manager->PostMessage(B_QUIT_REQUESTED);
	if(the_attr_manager)
		the_attr_manager->PostMessage(B_QUIT_REQUESTED); // only after windows are dead
	
	//printf("delete res_strings;\n");
	delete res_strings;
	
	//printf("return BApplication::QuitRequested();\n");
	return BApplication::QuitRequested();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//  AboutRequested
///////////////////

void AttrApp::AboutRequested()
{
/*
	BAlert *alert;
	alert = new BAlert("about",
	"Jellikit 0.9b1 by John Wiggins.\n" 
	"(most of)1998\n\n"
	"It took a long time to write...\n"
	"I redesigned and restructured constantly...\n"
	"But finally I was satisfied, and decided to\n"
	"release its fury upon the earth.\n"
	"enjoy :)\n\n"
	,"neat");
	alert->Go();
*/
	if(!about_box_alive)
	{
		BRect rect = BScreen().Frame(); // get the screen rect
		new JellikitAboutWindow(BRect((rect.Width()/2)-125, (rect.Height()/2)-75, (rect.Width()/2)+125, (rect.Height()/2)+75), media_server_is_alive); // the window should be 250 x 150
		about_box_alive = true;
		//printf("Your about box is showing.\n");
	}
	else
	{
		// activate the about box.
		const char *string_ptr = res_strings->String(STRING_ABOUT);
		BWindow *win = FindWindow(string_ptr);
		if(win != NULL)
			win->Activate(true);
		win = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//  ReadyToRun
///////////////////
void AttrApp::ReadyToRun()
{
	if(CountWindows() < 3) // EditorWindow should be alive
	{
		BMessage hello_world(ADD_FILE_TO_LIST), *parcel = new BMessage(NEW_WINDOW);
		hello_world.AddString("path", B_EMPTY_STRING);
		hello_world.AddInt32("win id", window_id_source);
		hello_world.AddPointer("msg ptr", (void *)parcel);
		if(AttrMessenger->IsValid())
			AttrMessenger->SendMessage(&hello_world);
		the_editor_window->AddToSubset(new AttrWindow(GetNewWindowRect(), B_EMPTY_STRING, parcel, (BLooper *)the_addon_manager, (BLooper *)the_attr_manager, the_editor_window, the_addattr_window, window_id_source++));
	}
	if(the_editor_window->Lock())
	{
		if(the_editor_window->IsHidden())
			the_editor_window->Show();
		the_editor_window->Unlock();
	}
	PostMessage(MAKE_FILE_PANELS); // kludge. lets the window get all situated before 
									// blocking the be_app thread with filepanel 
									// construction. Shame on Pavel.
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//  RefsReceived
///////////////////

void AttrApp::RefsReceived(BMessage *msg)
{
	//printf("AttrApp::RefsReceived()\n");
	BEntry		entry;
	BPath		path;
	BMessage	query(IS_FILE_OPEN),reply, hello_world(ADD_FILE_TO_LIST);
	entry_ref	file_ref;
	int32		message_num=0;
	//status_t	err;
	
	// check beforehand rather than for every damn loop :)
	if(the_editor_window->Lock())
	{
		if(the_editor_window->IsHidden())
			the_editor_window->Show();
		the_editor_window->Unlock();
	}
	
	// goddamn it was easy to make this open multiple files per drag&drop
	while(msg->FindRef("refs", message_num++, &file_ref) == B_OK)
	{
		entry.SetTo(&file_ref);
		if(entry.InitCheck() == B_NO_ERROR)
		{
			if(entry.GetPath(&path) == B_NO_ERROR)
			{
				// check to see if we already opened this file
				if(AttrMessenger->IsValid())
				{
					query.AddString("path", path.Path());
					AttrMessenger->SendMessage(&query, &reply);
					query.RemoveName("path"); // for the next loop
					if(reply.what == ANSWER_NO)
					{
						// the file is not currently open
						BNode node(path.Path());
						BMessage *parcel = new BMessage(NEW_WINDOW);
				
						if(node.InitCheck() == B_NO_ERROR)
						{
							attr_info a_info;
							char *buffer; // the attribute middleman
							char attribute_name[B_ATTR_NAME_LENGTH];
							uint32 attr_type;
							
							node.RewindAttrs();
							while(node.GetNextAttrName(attribute_name) == B_NO_ERROR)
							{
								if(node.GetAttrInfo(attribute_name, &a_info) == B_NO_ERROR)
								{
									attr_type = a_info.type;
									buffer = new char [ a_info.size ];
									if(node.ReadAttr(attribute_name, attr_type, 0, (void *)buffer, a_info.size) == a_info.size)
										parcel->AddData(attribute_name, attr_type, (void *)buffer, a_info.size, false);
									delete [] buffer;
									buffer = NULL;
								}
							}
					
							//parcel->PrintToStream();
					
							//delete parcel;					
							//parcel=NULL;
							hello_world.AddString("path", path.Path());
							hello_world.AddInt32("win id", window_id_source);
							hello_world.AddPointer("msg ptr", (void *)parcel);
							AttrMessenger->SendMessage(&hello_world);
							the_editor_window->AddToSubset(new AttrWindow(GetNewWindowRect(), path.Leaf(), parcel,(BLooper *)the_addon_manager, (BLooper *)the_attr_manager, the_editor_window, the_addattr_window, window_id_source++));
							hello_world.MakeEmpty(); // for the next loop
						}
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//  SaveRequested
///////////////////

void AttrApp::SaveRequested(BMessage *msg)
{
	BEntry		entry;
	BPath		path;
	entry_ref	file_ref;
	const char	*name_ptr;
	int32		win_id;
	//status_t	err;
	
	//printf("AttrApp::SaveRequested() : start\n");
	if(msg->FindRef("directory", 0, &file_ref) == B_OK)
	{
		//printf("AttrApp::SaveRequested() : found \"directory\" in the msg\n");
		entry.SetTo(&file_ref);
		if(entry.InitCheck() == B_NO_ERROR)
		{
			//printf("AttrApp::SaveRequested() : entry_ref to BEntry successful\n");
			if(entry.GetPath(&path) == B_NO_ERROR)
			{
				//printf("AttrApp::SaveRequested() : BEntry to path just peachy\n");
				if(msg->FindString("name", &name_ptr) == B_NO_ERROR)
				{
					if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
					{
						//printf("AttrApp::SaveRequested() : \"name\" found in msg\n");
						path.Append(name_ptr);
						msg->AddString("pathname", path.Path());
					
						// send the path off to loopers who care
						BMessage SetNameMsg(SET_PATH_FOR_FILE);
						SetNameMsg.AddString("pathname", path.Path());
						SetNameMsg.AddInt32("win id", win_id);
						AttrMessenger->SendMessage(&SetNameMsg);
						
						// do the save
						//printf("AttrApp::SaveRequested() : doing the save\n");
						status_t err = Save(msg);
						if(err < B_NO_ERROR)
							(new BAlert("alert", strerror(err),"ok"))->Go();
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//  MessageReceived
///////////////////

void AttrApp::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case WINDOW_REGISTRY_ADD:
		{
			bool need_id = false;

			if(msg->FindBool("need_id", &need_id) == B_OK)
			{
				if(need_id)
				{
					BMessage reply(WINDOW_ADDED);

					reply.AddInt32("new_window_number", next_untitled_number);
					msg->SendReply(&reply);
					next_untitled_number++;
				}
				window_count++;
			}
			break;
		}
		case WINDOW_REGISTRY_SUB:
		{
			// msg contains:
			// "win id" - unique id of window - int32
			// action:
			// - remove window from the_editor_window's subset and decrement window_count
			int32 win_id;
			BWindow *window = NULL;
			
			if(msg->FindInt32("win id", &win_id) == B_NO_ERROR)
				if((window = FindWindowByID(win_id)) != NULL)
					the_editor_window->RemoveFromSubset(window);
			window_count--;
			if(!window_count)
			{
				PostMessage(B_QUIT_REQUESTED);
			}
			break;
		}
		case ABOUT_BOX_DYING:
		{
			//printf("The about box is gone.\n");
			about_box_alive = false;
			break;
		}
		case B_SAVE_REQUESTED:
		{
			SaveRequested(msg);
			break;
		}
		case SAVE_AS_FILE:
		{
			// msg contains:
			// "win id" - unique window id of window that wants us to save its attrs
			// action:
			// - pass the message off to the Save: BFilePanel and it will send it back 
			//		with needed info
			//printf("SAVE_AS_FILE msg received\n");
			msg->what = B_SAVE_REQUESTED;
			SaveAsPanel->SetMessage(msg);
			SaveAsPanel->Show();
			break;
		}
		case SAVE_FILE:
		{
			// simple save
			//printf("AttrApp::MessageReceived(). got SAVE_FILE message.\n");
			// msg contains:
			// "win id" - unique window id of window that wants us to save its attrs
			// action:
			// - make sure the window has a filename associated with it
			// - if it doesn't, spawn the Save As window and break
			BMessage reply, query;
			const char *pathname;
			status_t err;
			
			query = *msg; // make a copy
			query.what = GET_PATH_FOR_FILE;
			AttrMessenger->SendMessage(&query, &reply);
			//printf("reply from AttrMessenger:\n");
			//reply.PrintToStream();
			if(reply.FindString("pathname", &pathname) == B_NO_ERROR)
			{
				//printf("strlen(pathname) = %d\n", strlen(pathname));
				if(strlen(pathname) == 0) // no filename (pathname is B_EMPTY_STRING)
				{
					//printf("strlen(pathname) = %d, and aparently that's < 1\n", i);
					msg->what = B_SAVE_REQUESTED;
					SaveAsPanel->SetMessage(msg);
					SaveAsPanel->Show();
				}
				else
				{
					//printf("saving file : %s\n", pathname);
					msg->AddString("pathname", pathname);
					err = Save(msg);
					if(err < B_NO_ERROR)
						(new BAlert("alert", strerror(err),"ok"))->Go();
				}
			}
			break;
		}
		case NEW_FILE:
		{
			BMessage hello_world(ADD_FILE_TO_LIST), *parcel = new BMessage(NEW_WINDOW);
			BMessenger messenger(the_attr_manager);
			hello_world.AddString("path", B_EMPTY_STRING);
			hello_world.AddInt32("win id", window_id_source);
			hello_world.AddPointer("msg ptr", (void *)parcel);
			if(AttrMessenger->IsValid())
				AttrMessenger->SendMessage(&hello_world);
			the_editor_window->AddToSubset(new AttrWindow(GetNewWindowRect(), B_EMPTY_STRING, parcel, (BLooper *)the_addon_manager, (BLooper *)the_attr_manager, the_editor_window, the_addattr_window, window_id_source++));
			break;
		}
		case OPEN_FILE:
		{
			OpenPanel->Show();
			break;
		}
		case MAKE_FILE_PANELS:
		{
			// woohoo. kludge for general sluggishness of BFilePanel instantiation
			OpenPanel = new BFilePanel();
			SaveAsPanel = new BFilePanel(B_SAVE_PANEL);
			break;
		}
		default:
			BApplication::MessageReceived(msg);
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//  Save
///////////////////

status_t AttrApp::Save(BMessage *msg)
{
	// msg contains "filename" and "attrmsg"
	const char *filename; // name of the file
	char *name; // the name of an attribute
	const void *ptr;
	BMessage *attrmsg, *remove_list, query_msg(GET_ATTRIBUTES), reply;
	BMessenger messenger(the_attr_manager);
	type_code type;
	int32 count,i,win_id;
	ssize_t size;
	status_t err = B_NO_ERROR;

	if(msg->FindString("pathname", &filename) == B_NO_ERROR)	
	{
		//printf("AttrApp::Save() flename = %s\n", filename);
		BFile file;
		
		if(msg->what == B_SAVE_REQUESTED) // this is a save as
			file.SetTo(filename, B_WRITE_ONLY|B_CREATE_FILE);
		else // this is a regular save
			file.SetTo(filename, B_WRITE_ONLY);
			
		if((err = file.InitCheck()) < B_NO_ERROR)
			return err;
	
		if((err = msg->FindInt32("win id", &win_id)) == B_NO_ERROR)
		{
			//printf("AttrApp::Save(). grabbed win_id.\n");
			// now talk to AttributeManager and get a pointer to the attributes
			query_msg.AddInt32("win id", win_id);
			messenger.SendMessage(&query_msg, &reply);
			if(reply.FindPointer("attrmsg", (void **)&attrmsg) == B_NO_ERROR) // this could fail
			{
				//attrmsg->PrintToStream();
			
				//printf("AttrApp::Save(). entering attribute writing loop.\n");
				for(i=0;attrmsg->GetInfo(B_ANY_TYPE,i,&name,&type,&count)==B_OK;i++)
				{
					//printf("AttrApp::Save(). writing attr: %s\n", name);
					if((err = attrmsg->FindData(name, type, &ptr, &size)) == B_NO_ERROR)
					{
						//if((err = file.RemoveAttr(name)) < B_NO_ERROR)
						//	return err;
						if((err = file.WriteAttr(name, type, 0, ptr, size)) < B_NO_ERROR)
							return err;
						//printf("%s\nptr = %s\n", strerror(err), ptr);
						ptr=NULL;
						//printf("success!\n");
					}
					else return err;
				}
				//printf("AttrApp::Save(). attribute writing loop complete.\n");
				//printf("%s\n",strerror(err));
			}
			if(reply.FindPointer("removelist", (void **)&remove_list) == B_NO_ERROR) // this could fail
			{
				for(i=0;remove_list->GetInfo(B_STRING_TYPE,i,&name,&type,&count)==B_OK;i++)
				{
					if((err = remove_list->FindData(name, type, &ptr, &size)) == B_NO_ERROR)
						file.RemoveAttr(name);
					ptr=NULL;
				}
			}
		}
	}
	//printf("AttrApp::Save(). all done.\n");
	attrmsg = NULL;
	remove_list = NULL;
	return err;	
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetNewWindowRect
///////////////////

BRect AttrApp::GetNewWindowRect()
{
	BRect screen_frame = BScreen().Frame();
	static int32 start_x=100, start_y=50;
	if(screen_frame.right < (start_x+WINDOW_X+20))
		start_x=50;
	else
		start_x+=20;
	if(screen_frame.bottom < (start_y+WINDOW_Y+20))
		start_y=50;
	else
		start_y+=20;
	return BRect(start_x, start_y, start_x + WINDOW_X, start_y + WINDOW_Y);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//  FindWindow
///////////////////

BWindow * AttrApp::FindWindow(const char *title)
{
	BWindow *win = NULL;
	int32 i = 0;
	while((win = WindowAt(i++)) != NULL)
	{
		if(win->Lock())
		{
			if((win->Title() != NULL) && !strcmp(win->Title(),title))
			{
				win->Unlock();
				return win;
			}
			win->Unlock();
		}
	}
	return win;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//  FindWindowByID
///////////////////

BWindow *AttrApp::FindWindowByID(int32 window_id)
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
					window->Unlock();
					return window;
				}
			window->Unlock();
		}
	
	return window;
}