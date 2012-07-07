#include "BoolAddon.h"

AttrAddon * Instantiate(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret)
{
	return new BoolAddon(frame,name,resizeMask,flags,msg,ret);
}

BoolAddon::BoolAddon(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret)
			: AttrAddon(frame,name,resizeMask,flags,msg,ret)
{
	//printf("BoolAddon: Instantiate.\n");
	BFont font(be_plain_font);
	int32 interfaceUnit = ((int32)font.Size()), width = frame.Width(), height = frame.Height();;
	
	// get "typecode", "data", & "datasize" out of the message and use
	// the data to fill in some stuff
	BRect rect;
	int32 datasize, typecode;
	ssize_t size_from_message;
	void *data; // for some reason, FindData hates for this to be NULL
	bool value;
	status_t err;
	if(msg->FindInt32("datasize", &datasize) == B_NO_ERROR)
	{
		//printf("BoolAddon: grabbed datasize\n");
		if(msg->FindInt32("typecode", &typecode) == B_NO_ERROR)
		{
			current_type = typecode; // kludge!!! 
			//printf("BoolAddon: grabbed typecode. typecode = %u\n", typecode);
			if((err = msg->FindPointer("data", &data)) == B_NO_ERROR)
			{
				//printf("BoolAddon: grabbed data\n");
				//printf("BoolAddon: all data harvested from message.\n");
				// everything's just dandy!
				memcpy((void *)&value, data, 1); // copy the byte
			}
			else
			{
				//printf("error = %s\n", strerror(err));
				*ret = B_ERROR;
				return;
			}
		}
	}
	//msg->PrintToStream();
	
	switch(current_type)
	{
		case B_BOOL_TYPE:
		{
			rect.Set(interfaceUnit, interfaceUnit, width - (interfaceUnit<<1), height - (interfaceUnit<<1));
			BRect textrect = rect;
			textrect.InsetBy(-1,-1);
			
			BTextView *textview = new BTextView(rect, "Bool", textrect, B_FOLLOW_ALL, B_WILL_DRAW/* | B_SCRIBBLE_ON_PRESCHOOLERS*/);
			BScrollView *scrollview = new BScrollView("scrollview", textview, B_FOLLOW_ALL, 0, false, true);
			initial_value = value;
			if(initial_value)
				textview->SetText("1");
			else
				textview->SetText("0");
			AddChild(scrollview);
			//rect.Set(interfaceUnit, interfaceUnit, width - interfaceUnit, interfaceUnit<<1);
//			BCheckBox *checkbox = new BCheckBox(rect, "Bool", "Bool", NULL);
//			((BControl*)checkbox)->SetValue((int32)value); // in case of bool, data is 0 or 1
//			
//			initial_value = value;
//			//printf("BoolAddon : AddChild(checkbox);\n");
//			AddChild(((BView*)checkbox));
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
	
#ifndef B_BEOS_VERSION_4
	SetViewColor(216,216,216,255);
#else
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
#endif
	//printf("BoolAddon : done!\n");
	*ret = B_NO_ERROR;
}

BoolAddon::~BoolAddon()
{
	// do nothing
}

status_t BoolAddon::GetData(BMessage *msg)
{
	// add feild "data" to message
	status_t err;
	switch(current_type)
	{
		case B_BOOL_TYPE:
		{
			BTextView *textview = (BTextView *)FindView("Bool");
			int16 value;
			bool ret_value;
	
			if(textview != NULL)
			{
				value = atoi(textview->Text());
				if(value)
					ret_value = true;
				else
					ret_value = false;
				err = msg->AddData("data", current_type, (void *)&ret_value, sizeof(bool));
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

status_t BoolAddon::ChangeData(BMessage *msg)
{
	// msg contains feild "data"
	status_t err;
	switch(current_type)
	{
		case B_BOOL_TYPE: // almost not worth checking
		{
			BTextView *textview = (BTextView *)FindView("Bool");
			if(textview != NULL)
			{
				char *data;
				ssize_t size;
				if((err = msg->FindData("data", current_type, (void**)&data, &size)) == B_NO_ERROR)
				{
					initial_value = (bool)*data;
					if(initial_value)
						textview->SetText("1");
					else
						textview->SetText("0");
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
bool BoolAddon::IsDirty() const
{
	BTextView *textview = (BTextView *)FindView("Bool");
	bool cur_value;
	int16 temp_value;
	if(textview != NULL)
	{
		temp_value = atoi(textview->Text());
		if(temp_value)
			cur_value = true;
		else
			cur_value = false;
		
		if(initial_value == cur_value)
			return false;
		else
			return true;
	}
	return false; // better to lie than corrupt :)
}