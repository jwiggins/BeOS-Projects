/*
	PluginManager.h
	All purpose add-on management
	2000 John Wiggins
*/

#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <app/Application.h>
#include <app/Roster.h>
#include <app/Message.h>
#include <app/Messenger.h>
#include <app/Looper.h>
#include <storage/Path.h>
#include <storage/Entry.h>
#include <storage/Directory.h>
#include <support/List.h>
#include <support/String.h>
#include <kernel/image.h>
#include <kernel/OS.h>
#include <stdio.h>
#include <string.h>

#include "JEPlugin.h"
#include "MessageConstants.h"

// list item for list of currently loaded images 
struct image_item {
	char *name;
	int32 refcount;
	image_id id;
};
// list item for list of plugin instances
struct plugin_item {
	char *name;
	void *instance;
};

class PluginManager : public BLooper {
public:
						PluginManager(thread_id src1, thread_id src2, BMessenger *pipeline);
						~PluginManager();
						
	virtual	void		MessageReceived(BMessage *msg);

private:
	status_t			LoadPlugin(const char *path, int32 index, void *args, void **addn);
	status_t			InstantiatePlugin(const char *path, je_plugin *addn, void *args, void **inst);
	bool				IsLoaded(const char *path);
	void				UnloadPlugin(void *inst);
	void				ScanPlugins();
	void				DiscoverPlugins(je_plugin **list, const char *image);
	
	BString				pluginDir;
	BMessenger			*graphicsPipeline;
	BMessenger			*defaultOutWin;
	thread_id			source1, source2;
	BMessage			*sources, *mixers, *filters, *outputs;
	BList				imageLoadList;
	BList				pluginLoadList;
};

#endif