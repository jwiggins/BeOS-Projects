/*
	PluginOutlineListView.h
	PluginOutlineListView class declaration for Jeepers Elvis!
	2000 John Wiggins
*/

#ifndef PLUGIN_OUTLINE_LIST_VIEW_H
#define PLUGIN_OUTLINE_LIST_VIEW_H

#include <interface/OutlineListView.h>
#include <interface/ListItem.h>
#include <interface/Rect.h>
#include <interface/Point.h>
#include <interface/Bitmap.h>
#include <app/Application.h>
#include <app/Message.h>
#include <storage/Resources.h>

#include "MessageConstants.h"
#include "JEPlugin.h"

class PluginOutlineListView : public BOutlineListView {
public:
							PluginOutlineListView(BRect frame, const char *name);
							~PluginOutlineListView();
	
	virtual	void			MessageReceived(BMessage *msg);
	virtual	bool			InitiateDrag(BPoint pt, int32 index, bool selected);

private:
	void					PopulateList();
	
	class PluginListItem : public BStringItem {
	public:
							PluginListItem(const char *label, int32 _type, int32 _index,
								uint32 level = 1, bool expanded = false);
	
	// data members (public)
		int32				type;
		int32				index;
	};

	BBitmap					*sourceIcon, *mixerIcon, *filterIcon, *outputIcon;
	BMessage				*sources, *mixers, *filters, *outputs;
	
};

#endif