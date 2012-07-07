/*
	PluginOutlineListView.cpp
	2000 John Wiggins
*/

#include "PluginOutlineListView.h"
#include "JEApp.h"

/////////////////////////
// PluginOutlineListView
/////////////////////////
PluginOutlineListView::PluginOutlineListView(BRect frame, const char *name)
: BOutlineListView(frame, name, B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL)
{
	BResources *resources = be_app->AppResources();
	BRect iconRect(0.0,0.0,31.0,31.0);
	const void *iconBits;
	size_t bitsSize;
	
	// get the source icon
	iconBits = resources->LoadResource('ICON', 1, &bitsSize);
	sourceIcon = new BBitmap(iconRect, B_CMAP8);
	sourceIcon->SetBits(iconBits, bitsSize, 0, B_CMAP8);
	
	// get the mixer icon
	iconBits = resources->LoadResource('ICON', 2, &bitsSize);
	mixerIcon = new BBitmap(iconRect, B_CMAP8);
	mixerIcon->SetBits(iconBits, bitsSize, 0, B_CMAP8);
	
	// get the filter icon
	iconBits = resources->LoadResource('ICON', 3, &bitsSize);
	filterIcon = new BBitmap(iconRect, B_CMAP8);
	filterIcon->SetBits(iconBits, bitsSize, 0, B_CMAP8);
	
	// get the output icon
	iconBits = resources->LoadResource('ICON', 4, &bitsSize);
	outputIcon = new BBitmap(iconRect, B_CMAP8);
	outputIcon->SetBits(iconBits, bitsSize, 0, B_CMAP8);
	
	// ain't there yet
	sources = NULL;
	mixers = NULL;
	filters = NULL;
	outputs = NULL;
}

PluginOutlineListView::~PluginOutlineListView()
{
	delete sourceIcon;
	delete mixerIcon;
	delete filterIcon;
	delete outputIcon;
	
	// we're in charge of freeing our items
	for (int32 i=0; i < CountItems(); i++)
	{
		BListItem *item = RemoveItem(0L);
		delete item;
	}
}

void PluginOutlineListView::MessageReceived(BMessage *msg)
{
	//printf("PluginOutlineListView::MessageReceived()\n");
	switch(msg->what)
	{
		case JE_GET_AVAILABLE_PLUGINS:
		{
			//printf("PluginOutlineListView::MessageReceived(JE_GET_AVAILABLE_PLUGINS)\n");
			// this is from the PluginManager
			void *message;
			
			if (msg->FindPointer("sources", &message) == B_OK)
			{
				sources = reinterpret_cast<BMessage *>(message);
				if (msg->FindPointer("mixers", &message) == B_OK)
				{
					mixers = reinterpret_cast<BMessage *>(message);
					if (msg->FindPointer("filters", &message) == B_OK)
					{
						filters = reinterpret_cast<BMessage *>(message);
						if (msg->FindPointer("outputs", &message) == B_OK)
						{
							outputs = reinterpret_cast<BMessage *>(message);
							
						}
					}
				}
				
				// fill in the list
				PopulateList();
			}
			
			break;
		}
		default:
		{
			BOutlineListView::MessageReceived(msg);
			break;
		}
	}
}

bool PluginOutlineListView::InitiateDrag(BPoint pt, int32 selection, bool selected)
{
	// start a drag
	PluginListItem *item = (PluginListItem *)ItemAt(selection);
			
	// make sure it's a draggable item
	if (item != NULL && item->type != 0)
	{
		// initiate a drag
		BMessage drag;
		BBitmap *bitmap = NULL;
		
		// fill in the message
		switch(item->type)
		{
			case JE_SOURCE_PLUGIN:
			{
				const char *name;
				const char *desc;
				int32 index;
				
				if (sources->FindString("name", item->index, &name) == B_OK)
				{
					if (sources->FindString("desc", item->index, &desc) == B_OK)
					{
						if (sources->FindInt32("index", item->index, &index) == B_OK)
						{
							drag.AddString("name", name);
							drag.AddString("desc", desc);
							drag.AddInt32("index", index);
						}
					}
				}
				
				// the bitmap
				bitmap = sourceIcon;
				// the what constant
				drag.what = JE_SOURCE_DND;
				break;
			}
			case JE_MIXER_PLUGIN:
			{
				const char *name;
				const char *desc;
				int32 index;
				
				if (mixers->FindString("name", item->index, &name) == B_OK)
				{
					if (mixers->FindString("desc", item->index, &desc) == B_OK)
					{
						if (mixers->FindInt32("index", item->index, &index) == B_OK)
						{
							drag.AddString("name", name);
							drag.AddString("desc", desc);
							drag.AddInt32("index", index);
						}
					}
				}
				
				// the bitmap
				bitmap = mixerIcon;
				// the what constant
				drag.what = JE_MIXER_DND;
				break;
			}
			case JE_FILTER_PLUGIN:
			{
				const char *name;
				const char *desc;
				int32 index;
				
				if (filters->FindString("name", item->index, &name) == B_OK)
				{
					if (filters->FindString("desc", item->index, &desc) == B_OK)
					{
						if (filters->FindInt32("index", item->index, &index) == B_OK)
						{
							drag.AddString("name", name);
							drag.AddString("desc", desc);
							drag.AddInt32("index", index);
						}
					}
				}
				
				// the bitmap
				bitmap = filterIcon;
				// the what constant
				drag.what = JE_FILTER_DND;
				break;
			}
			case JE_OUTPUT_PLUGIN:
			{
				const char *name;
				const char *desc;
				int32 index;
				
				if (outputs->FindString("name", item->index, &name) == B_OK)
				{
					if (outputs->FindString("desc", item->index, &desc) == B_OK)
					{
						if (outputs->FindInt32("index", item->index, &index) == B_OK)
						{
							drag.AddString("name", name);
							drag.AddString("desc", desc);
							drag.AddInt32("index", index);
						}
					}
				}
				
				// the bitmap
				bitmap = outputIcon;
				// the what constant
				drag.what = JE_OUTPUT_DND;
				break;
			}
			default:
				break;
		}
		
		// and drag!
		DragMessage(&drag, new BBitmap(bitmap), B_OP_OVER, BPoint(15.0,15.0));
	}
	
	return true;
}

void PluginOutlineListView::PopulateList()
{
	int32 index, count;
	type_code type;
	const char *label;
	JEApp *app = (JEApp *)be_app;
	PluginListItem *srcItem, *mixItem, *filtItem, *outItem;
	
	// first the sources
	sources->GetInfo("desc", &type, &count);
	AddItem(srcItem = new PluginListItem(app->ResString(STRING_SOURCES), 0, 0, 0, true));
	for (index = 0; index < count; index++)
	{
		if (sources->FindString("desc", index, &label) == B_OK)
		{
			PluginListItem *item = new PluginListItem(label, JE_SOURCE_PLUGIN, index);
			AddUnder(item, srcItem);
		}
	}
	
	// then the mixers
	mixers->GetInfo("desc", &type, &count);
	AddItem(mixItem = new PluginListItem(app->ResString(STRING_MIXERS), 0, 0, 0, true));
	for (index = 0; index < count; index++)
	{
		if (mixers->FindString("desc", index, &label) == B_OK)
		{
			PluginListItem *item = new PluginListItem(label, JE_MIXER_PLUGIN, index);
			AddUnder(item, mixItem);
		}
	}
	
	// then the filters
	filters->GetInfo("desc", &type, &count);
	AddItem(filtItem = new PluginListItem(app->ResString(STRING_FILTERS), 0, 0, 0, true));
	for (index = 0; index < count; index++)
	{
		if (filters->FindString("desc", index, &label) == B_OK)
		{
			PluginListItem *item = new PluginListItem(label, JE_FILTER_PLUGIN, index);
			AddUnder(item, filtItem);
		}
	}
	
	// and last the outputs
	outputs->GetInfo("desc", &type, &count);
	AddItem(outItem = new PluginListItem(app->ResString(STRING_OUTPUTS), 0, 0, 0, true));
	for (index = 0; index < count; index++)
	{
		if (outputs->FindString("desc", index, &label) == B_OK)
		{
			PluginListItem *item = new PluginListItem(label, JE_OUTPUT_PLUGIN, index);
			AddUnder(item, outItem);
		}
	}
}

//////////////////
// PluginListItem
//////////////////
PluginOutlineListView::PluginListItem::PluginListItem(const char *label, int32 _type, int32 _index, uint32 level, bool expanded)
: BStringItem(label, level, expanded)
{
	type = _type;
	index = _index;
}

