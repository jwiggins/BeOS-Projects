#include "IntegerAddon.h"

AttrAddon * Instantiate(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret)
{
	return new IntegerAddon(frame,name,resizeMask,flags,msg,ret);
}

IntegerAddon::IntegerAddon(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret)
			: AttrAddon(frame,name,resizeMask,flags,msg,ret)
{
	//printf("IntegerAddon: Instantiate.\n");
	BFont font(be_plain_font);
	int32 interfaceUnit = ((int32)font.Size()), width = frame.Width(), height = frame.Height();
	
	// get "typecode", "data", & "datasize" out of the message and use
	// the data to fill in some stuff
	BRect rect;
	int32 datasize, typecode;
	const void *data;
	status_t err;
	if(msg->FindInt32("datasize", &datasize) == B_NO_ERROR)
	{
		//printf("IntegerAddon: grabbed datasize\n");
		if(msg->FindInt32("typecode", &typecode) == B_NO_ERROR)
		{
			current_type = typecode; // kludge!!! 
			//printf("IntegerAddon: grabbed typecode. typecode = %u\n", typecode);
			if((err = msg->FindPointer("data", &data)) == B_NO_ERROR)
			{
				//printf("IntegerAddon: all data harvested from message.\n");
				// everything's just dandy!				
				
				// copy the data
				memcpy((void *)&current_data, data, datasize);
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
	
	// build the text view that holds the number
	rect.Set(interfaceUnit, interfaceUnit, width - (interfaceUnit<<1), height - (interfaceUnit<<1));
	BRect textrect = rect;
	char integer_string[32]; // how many digits can an int64 be?
	textrect.InsetBy(-1,-1);
	BTextView *textview = new BTextView(rect, "Integer", textrect, B_FOLLOW_ALL, B_WILL_DRAW/* | B_SCRIBBLE_ON_PRESCHOOLERS*/);
	BScrollView *scrollview = new BScrollView("scrollview", textview, B_FOLLOW_ALL, 0, false, true);
	
	// fill it with data
	switch(current_type)
	{
		/*case B_INT64_TYPE:
		{
			sprintf(integer_string, "%Ld", current_data);
			textview->SetText(integer_string);
			AddChild(scrollview);
			*ret = B_NO_ERROR;
			break;
		}*/
		case B_INT32_TYPE:
		{
			sprintf(integer_string, "%ld", current_data);
			textview->SetText(integer_string);
			AddChild(scrollview);
			*ret = B_NO_ERROR;
			break;
		}
		case B_INT16_TYPE:
		case B_INT8_TYPE:
		{
			sprintf(integer_string, "%d", current_data);
			textview->SetText(integer_string);
			AddChild(scrollview);
			*ret = B_NO_ERROR;
			break;
		}
		default:
		{
			// this shouldn't happen. but then hell _could_ freeze over.
			delete scrollview;
			rect.Set(interfaceUnit, interfaceUnit, width - interfaceUnit, interfaceUnit<<1);
			BStringView *stringview = new BStringView(rect, "Bad", "Bad Addon Instantiation");
			AddChild(stringview);
			*ret = B_ERROR;
			break;
		}
	}
	//printf("%s\n", integer_string);
	if(*ret < B_NO_ERROR)
		return;

// set the background color
#ifndef B_BEOS_VERSION_4
	SetViewColor(216,216,216,255);
#else
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
#endif
	//printf("IntegerAddon : done!\n");
	*ret = B_NO_ERROR;
}

IntegerAddon::~IntegerAddon()
{
	// nothing to do
}

status_t IntegerAddon::GetData(BMessage *msg)
{
	// add feild "data" to message
	status_t err = B_ERROR;
	int32 len;
	const char *text;
	BTextView *textview = (BTextView *)FindView("Integer");
	
	if(textview != NULL)
	{
		text = textview->Text();
		if(text != NULL)
		{
			//printf("packaging up attribute. Text in view = %s\n", text);
			switch(current_type)
			{
				case B_INT32_TYPE:
				{
					len = 4;
					int32 val = atol(text);
					//printf("val = %d\n", val);
					err = msg->AddData("data", current_type, (void *)&val, len);
					break;
				}
				case B_INT16_TYPE:
				{	
					len = 2;
					int16 val = atoi(text);
					//printf("val = %d\n", val);
					err = msg->AddData("data", current_type, (void *)&val, len);
					break;
				}
				case B_INT8_TYPE:
				{
					len = 1;
					int8 val = atoi(text);
					//printf("val = %d\n", val);
					err = msg->AddData("data", current_type, (void *)&val, len);
					break;
				}
				default:
				{
					err = B_ERROR;
					break;
				}
			}
		}
		else
			return B_ERROR;
	}
	else
		return B_ERROR;
	return err;
}

status_t IntegerAddon::ChangeData(BMessage *msg)
{
	// msg contains feild "data"
	status_t err = B_ERROR;
	void *data;
	ssize_t size;
	char integer_string[32], *buffer;
	BTextView *textview = (BTextView *)FindView("Integer");
	
	if(msg->FindData("data", current_type, &data, &size) == B_NO_ERROR)
	{
		buffer = new char [size]; // allocate
		memcpy((void *)buffer, data, size); // copy
	}
	else
		return B_ERROR;
	if(textview != NULL) // make sure the text view is really there
	{
		switch(current_type)
		{
			case B_INT32_TYPE:
			{
				current_data = (int32)(*buffer);
				sprintf(integer_string, "%ld", (int32)(*buffer));
				textview->SetText(integer_string);
				err = B_NO_ERROR;
				break;
			}
			case B_INT16_TYPE:
			{
				current_data = (int16)(*buffer);
				sprintf(integer_string, "%d", (int16)(*buffer));
				textview->SetText(integer_string);
				err = B_NO_ERROR;
				break;
			}
			case B_INT8_TYPE:
			{
				current_data = (int8)(*buffer);
				sprintf(integer_string, "%d", (int8)(*buffer));
				textview->SetText(integer_string);
				err = B_NO_ERROR;
				break;
			}
			default:
			{
				err = B_ERROR;
				break;
			}
		}		
	}
	else
		return B_ERROR;
	
	return err;
}

bool IntegerAddon::IsDirty() const
{
	//printf("IntegerAddon::IsDirty() : initial_string = %s\n", initial_string);
	BTextView *textview = (BTextView *)FindView("Integer");
	//const char *integer_string;
	if(textview != NULL)
	{
		switch(current_type)
		{
			case B_INT32_TYPE:
			{
				return !(current_data == atol(textview->Text()));
			}
			case B_INT16_TYPE:
			case B_INT8_TYPE:
			{
				return !(current_data == atoi(textview->Text()));
			}
			default:
				return false;
		}		
	}
	else
		return false; // if it's NULL, go ahead and fail. No data is better than munged data
}