#include "BRectAddon.h"

// in an attribute a BRect is just a 16 byte chunk
// I'm know that's right-top-bottom-left (trial and error)
// What I don't know is whether or not i should swap before I write


AttrAddon * Instantiate(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret)
{
	return new BRectAddon(frame,name,resizeMask,flags,msg,ret);
}

BRectAddon::BRectAddon(BRect frame, const char *name, uint32 resizeMask, uint32 flags, BMessage *msg, status_t *ret)
			: AttrAddon(frame,name,resizeMask,flags,msg,ret)
{
	//printf("BRectAddon: Instantiate.\n");
	BFont font(be_plain_font);
	int32 width = frame.Width(), height = frame.Height();
	
	int32 interfaceUnit = ((int32)font.Size());
	
	// get "typecode", "data", & "datasize" out of the message and use
	// the data to fill in some stuff
	BRect rect;
	int32 datasize, typecode;
	ssize_t size_from_message;
	char *data; // for some reason, FindData hates for this to be NULL
	status_t err;
	if(msg->FindInt32("datasize", &datasize) == B_NO_ERROR)
	{
		//printf("BRectAddon: grabbed datasize\n");
		if(msg->FindInt32("typecode", &typecode) == B_NO_ERROR)
		{
			current_type = typecode; // kludge!!! 
			//printf("BRectAddon: grabbed typecode. typecode = %u\n", type);
			if((err = msg->FindPointer("data", (void **)&data)) == B_NO_ERROR)
			{
				//printf("BRectAddon: all data harvested from message.\n");
				// everything's just dandy!
				// lets go ahead and fill it in here
				union {
					float vals[4];
					char ptr[16];
				};
				memcpy(ptr, data, 16);
				the_rect.Set(vals[0],vals[1],vals[2],vals[3]);
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
		case B_RECT_TYPE:
		{
			char string[9];
			int32 dividerwidth = StringWidth("Bottom");
			BTextControl *textview1, *textview2, *textview3, *textview4;
			
			rect.Set((width>>1)-(width>>2), interfaceUnit>>1, (width>>1)+(width>>2), interfaceUnit<<1);
			textview1 = new BTextControl(rect, "Top", "Top", NULL, NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP);
			
			rect.Set(interfaceUnit>>1, (interfaceUnit<<1)+(interfaceUnit), (width>>1)-2, (interfaceUnit<<2)+(interfaceUnit>>1));
			textview2 = new BTextControl(rect, "Left", "Left", NULL, NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP);
			
			rect.Set((width>>1)+2, (interfaceUnit<<1)+(interfaceUnit), width-(interfaceUnit>>1), (interfaceUnit<<2)+(interfaceUnit>>1));
			textview3 = new BTextControl(rect, "Right", "Right", NULL, NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP);
			
			rect.Set((width>>1)-(width>>2), (interfaceUnit<<2)+(interfaceUnit)+(interfaceUnit>>1), (width>>1)+(width>>2), (interfaceUnit<<2)+(interfaceUnit<<2));
			textview4 = new BTextControl(rect, "Bottom", "Bottom", NULL, NULL, B_FOLLOW_LEFT|B_FOLLOW_TOP);
			
			textview1->SetDivider(dividerwidth + 2);
			textview2->SetDivider(dividerwidth + 2);
			textview3->SetDivider(dividerwidth + 2);
			textview4->SetDivider(dividerwidth + 2);
			textview1->SetAlignment(B_ALIGN_CENTER, B_ALIGN_LEFT);
			textview2->SetAlignment(B_ALIGN_CENTER, B_ALIGN_LEFT);
			textview3->SetAlignment(B_ALIGN_CENTER, B_ALIGN_LEFT);
			textview4->SetAlignment(B_ALIGN_CENTER, B_ALIGN_LEFT);
			sprintf(string, "%8.4f", the_rect.top);
			textview1->SetText(string);
			sprintf(string, "%8.4f", the_rect.left);
			textview2->SetText(string);
			sprintf(string, "%8.4f", the_rect.right);
			textview3->SetText(string);
			sprintf(string, "%8.4f", the_rect.bottom);
			textview4->SetText(string);
			//printf("BRectAddon : view->AddChild(textcontrol);\n");
			AddChild(textview1);
			AddChild(textview2);
			AddChild(textview3);
			AddChild(textview4);
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
	//printf("BRectAddon : done!\n");
	*ret = B_NO_ERROR;
}

BRectAddon::~BRectAddon()
{

}

status_t BRectAddon::GetData(BMessage *msg)
{
	// add feild "data" to message
	status_t err;
	int32 len;
	switch(current_type)
	{
		case B_RECT_TYPE:
		{
			BTextControl *textview;
			union {
				float vals[4];
				char ptr[16];
			};
			textview = (BTextControl *)FindView("Left");
			if(textview != NULL)
				vals[0] = atof(textview->Text());
			else
			{
				err = B_ERROR;
				break;
			}
			textview = (BTextControl *)FindView("Top");
			if(textview != NULL)
				vals[1] = atof(textview->Text());
			else
			{
				err = B_ERROR;
				break;
			}
			textview = (BTextControl *)FindView("Right");
			if(textview != NULL)
				vals[2] = atof(textview->Text());
			else
			{
				err = B_ERROR;
				break;
			}
			textview = (BTextControl *)FindView("Bottom");
			if(textview != NULL)
				vals[3] = atof(textview->Text());
			else
			{
				err = B_ERROR;
				break;
			}
			// all righty. all the fun data is now in ptr[16]
			err = msg->AddData("data", current_type, (void *)ptr, 16);
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

status_t BRectAddon::ChangeData(BMessage *msg)
{
	// msg contains feild "data"
	status_t err;
	switch(current_type)
	{
		case B_RECT_TYPE:
		{
			BTextControl *textview;
			char string[9];
			union {
				float vals[4];
				char ptr[16];
			};
			void *data;
			ssize_t size;
			
			if((err = msg->FindData("data", current_type, &data, &size)) == B_NO_ERROR)
			{
				memcpy(ptr, data, 16);
				the_rect.Set(vals[0],vals[1],vals[2],vals[3]);
				textview = (BTextControl *)FindView("Left");
				if(textview != NULL)
				{
					sprintf(string, "%8.4f", the_rect.left);
					textview->SetText(string);
				}
				else
				{
					err = B_ERROR;
					break;
				}
				textview = (BTextControl *)FindView("Top");
				if(textview != NULL)
				{
					sprintf(string, "%8.4f", the_rect.top);
					textview->SetText(string);
				}
				else
				{
					err = B_ERROR;
					break;
				}
				textview = (BTextControl *)FindView("Right");
				if(textview != NULL)
				{
					sprintf(string, "%8.4f", the_rect.right);
					textview->SetText(string);
				}
				else
				{
					err = B_ERROR;
					break;
				}
				textview = (BTextControl *)FindView("Bottom");
				if(textview != NULL)
				{
					sprintf(string, "%8.4f", the_rect.bottom);
					textview->SetText(string);
				}
				else
				{
					err = B_ERROR;
					break;
				}
			}
			else
				err = B_ERROR;
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

bool BRectAddon::IsDirty() const
{
	//printf("BRectAddon::IsDirty()\n");
	return false; // if it's NULL, go ahead and fail. No data is better than munged data
}