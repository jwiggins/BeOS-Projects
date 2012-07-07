#include "StringAddon.h"

AttrAddon * Instantiate(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret)
{
	return new StringAddon(frame,name,resizeMask,flags,msg,ret);
}

StringAddon::StringAddon(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret)
			: AttrAddon(frame,name,resizeMask,flags,msg,ret)
{
	//printf("StringAddon: Instantiate.\n");
	BFont font(be_plain_font);
	int32 interfaceUnit = ((int32)font.Size()), width = frame.Width(), height = frame.Height();
	
	// get "typecode", "data", & "datasize" out of the message and use
	// the data to fill in some stuff
	BRect rect;
	int32 datasize, typecode;
	ssize_t size_from_message;
	const void *data; // for some reason, FindData hates for this to be NULL
	status_t err;
	if(msg->FindInt32("datasize", &datasize) == B_NO_ERROR)
	{
		//printf("StringAddon: grabbed datasize\n");
		if(msg->FindInt32("typecode", &typecode) == B_NO_ERROR)
		{
			current_type = typecode; // kludge!!! 
			//printf("StringAddon: grabbed typecode. typecode = %u\n", type);
			if((err = msg->FindPointer("data", &data)) == B_NO_ERROR)
			{
				//printf("StringAddon: all data harvested from message.\n");
				// everything's just dandy!				
				// make a copy of the string.
				initial_string = new char [ datasize + 1]; // allocate
				strcpy(initial_string, (char *)data); // copy. yay.
				//printf("StringAddon: initial_string = %s\n", initial_string);
			}
			else
			{
				//printf("error = %s\n", strerror(err));
				*ret = B_ERROR; // general error. The Edit Window knows what to do
				return;
			}
		}
	}
	//msg->PrintToStream();
	
	if(initial_string != NULL)
	{
		switch(current_type)
		{
			case B_STRING_TYPE:
			case B_MIME_STRING_TYPE:
			{
				rect.Set(interfaceUnit, interfaceUnit, width - (interfaceUnit<<1), height - (interfaceUnit<<1));
				BRect textrect = rect;
				textrect.InsetBy(-1,-1);
				//BTextControl *textview = new BTextControl(rect, "String", "String", NULL, NULL, B_FOLLOW_ALL);
				BTextView *textview = new BTextView(rect, "String", textrect, B_FOLLOW_ALL, B_WILL_DRAW/* | B_SCRIBBLE_ON_PRESCHOOLERS*/);
				BScrollView *scrollview = new BScrollView("scrollview", textview, B_FOLLOW_ALL, 0, false, true);
				textview->SetText(initial_string);
				//textview->SetDivider(StringWidth("String")+2);
				//printf("StringAddon : view->AddChild(textcontrol);\n");
				//AddChild(textview);
				AddChild(scrollview);
				break;
			}
			default:
			{
				// this shouldn't happen. but then hell _could_ freeze over.
				rect.Set(interfaceUnit, interfaceUnit, width - interfaceUnit, interfaceUnit<<1);
				BStringView *stringview = new BStringView(rect, "Bad Addon Instantiation", "Bad Addon Instantiation");
				AddChild(stringview);
				break;
			}
		}
#if B_BEOS_VERSION < B_BEOS_VERSION_4
	SetViewColor(216,216,216,255);
#else
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
#endif
	}
	//printf("StringAddon : done!\n");
	*ret = B_NO_ERROR;
}

StringAddon::~StringAddon()
{
	// delete cached string
	delete [] initial_string;
	initial_string = NULL;
}

status_t StringAddon::GetData(BMessage *msg)
{
	// add feild "data" to message
	status_t err;
	int32 len;
	switch(current_type)
	{
		case B_STRING_TYPE:
		case B_MIME_STRING_TYPE:
		{
			BTextView *textview = (BTextView *)FindView("String");
	
			if(textview != NULL)
			{
				const char *text = textview->Text();
				if(text != NULL)
				{
					len = strlen(text) + 1;
					err = msg->AddData("data", current_type, (void *)text, len);
				}
				else
				{
					err = B_ERROR;
				}
				//err = B_NO_ERROR;
			}
			else
			{
				//buffer = NULL;
				//len = 0;
				err = B_ERROR;
			}
			break;
		}
		default:
		{
			//buffer = NULL;
			//len = 0;
			err = B_ERROR;
			break;
		}
	}
	return err;
}

status_t StringAddon::ChangeData(BMessage *msg)
{
	// msg contains feild "data"
	status_t err;
	switch(current_type)
	{
		case B_STRING_TYPE:
		case B_MIME_STRING_TYPE:
		{
			BTextView *textview = (BTextView *)FindView("String");
			if(textview != NULL)
			{
				void *data;
				ssize_t size;
				if((err = msg->FindData("data", current_type, &data, &size)) == B_NO_ERROR)
				{
					textview->SetText((char *)data); // set text
					delete [] initial_string; // deallocate for fresh copy
					initial_string = NULL;
					initial_string = new char [ strlen((char *)data) + 1]; // allocate
					//memset((void*)initial_string, 0, strlen(initial_string)); // appease malloc gods
					strcpy(initial_string, (char *)data); // copy. yay.					
				}
				//err = B_NO_ERROR;
			}
			else
			{
				err = B_ERROR;
			}
			break;
		}
		default:
		{
			err = B_ERROR;
			break;
		}
	}
	return err;
}

bool StringAddon::IsDirty() const
{
	printf("StringAddon::IsDirty() : initial_string = %s\n", initial_string);
	BTextView *textview = (BTextView *)FindView("String");
	if(textview != NULL)
	{
		if(!strcmp(initial_string, textview->Text()))
			return false;
		else
			return true;
	}
	else
		return false; // if it's NULL, go ahead and fail. No data is better than munged data
}