/*
	MessageConstants.h
	BMessage::what constants for Jeepers Elvis!
	2000 John Wiggins
*/

#ifndef MESSAGE_CONSTANTS_H
#define MESSAGE_CONSTANTS_H

#include "PluginMessages.h"

enum {
	JE_WINDOW_REGISTRY_ADD = 'wadd',
	JE_WINDOW_REGISTRY_SUB = 'wsub',
	JE_SEND_EMAIL = 'send',
	JE_INIT_TIMER = 'intm',
	JE_APP_IS_QUITTING = 'aiqt',
	JE_GENERATE_FRAME = 'gnfm',
	JE_SET_DEFAULT_OUTPUT = 'sdot',
	JE_UNSET_DEFAULT_OUTPUT = 'udot',
	JE_SET_SOURCE_OUTPUTS = 'ssot',
	//JE_DRAW_BITMAP = 'draw', // now in PluginMessages.h
	//JE_FRAMERATE_CHANGED = 'fmrt', // now in PluginMessages.h
	//JE_FRAME_SIZE_CHANGED = 'fmsz', // now in PluginMessages.h
	JE_SOURCE_CHANGED = 'scch',
	//JE_SOURCE_READY = 'scrd', // now in PluginMessages.h
	JE_START_SOURCE = 'stsc',
	JE_STOP_SOURCE = 'spsc',
	JE_MIXER_CHANGED = 'mxch',
	JE_OUTPUT_CHANGED = 'otch',
	JE_FILTER_ADDED = 'ftad',
	JE_MOVE_FILTER = 'ftmv',
	JE_GET_UI = 'ggui',
	JE_LOAD_UI = 'ldui',
	JE_UNLOAD_UI = 'ului',
	JE_GET_AVAILABLE_PLUGINS = 'gapi',
	JE_LOAD_PLUGIN = 'ldpi',
	JE_UNLOAD_PLUGIN = 'ulpi',
	JE_PLUGIN_LOADED = 'pild',
	JE_SOURCE_DND = 'sdnd',
	JE_MIXER_DND = 'mdnd',
	JE_FILTER_DND = 'fdnd',
	JE_OUTPUT_DND = 'odnd',
	JE_CONTAINER_VIEW_DND = 'cvdd'
};

/*
	Fields found in above messages:
	
	JE_WINDOW_REGISTRY_ADD:
		- none
	JE_WINDOW_REGISTRY_SUB:
		- none
	JE_SEND_EMAIL:
		- none
	JE_APP_IS_QUITTING:
		- none
	JE_GENERATE_FRAME:
		- none
	JE_SET_DEFAULT_OUTPUT:
		- "window" : pointer to a BHandler derived class. More specifically a
		pointer to a window that serves as default output.
	JE_UNSET_DEFAULT_OUTPUT:
		- none
	JE_DRAW_BITMAP:
		- "bitmap" : a pointer to a BBitmap. Receiver should blit the bitmap
		to the screen. (or some other suitable device :)
	JE_FRAMERATE_CHANGED:
		- "framerate" : a bigtime_t representing the new framerate. Value
		reflects the lifespan of a frame. (ex: 33000 would be ~30 fps)
		- "which" : an int32 specifying which source's framerate is changing.
		0 for Source1, 1 for Source2.
	JE_FRAME_SIZE_CHANGED:
		- "which" : an int32 specifying which source's frame size is changing.
		0 for Source1, 1 for Source2.
	JE_SOURCE_CHANGED:
		- "plugin" : a pointer to a je_source_plugin. This is a freshly loaded
		source plugin. No assumptions should be made about the internal state of
		the plugin instance. It will send a JE_SOURCE_READY message when it is
		ready to be a plugin :).
		- "which" : an int32 specifying which source is changing.
		0 for Source1, 1 for Source2.
	JE_SOURCE_READY:
		- "which" : an int32 specifying which source is ready.
		0 for Source1, 1 for Source2.
	JE_START_SOURCE:
		- "which" : an int32 specifying which source is being started.
		0 for Source1, 1 for Source2.
	JE_STOP_SOURCE:
		- "which" : an int32 specifying which source is being stopped.
		0 for Source1, 1 for Source2.
	JE_MIXER_CHANGED:
		- "plugin" : a pointer to a je_mixer_plugin. This is a freshly loaded
		mixer plugin. It should be completely ready for anything you throw at
		it. (within the bounds of a mixer plugin's purpose of course)
	JE_OUTPUT_CHANGED:
		- "plugin" : a pointer to a je_output_plugin. This is a freshly loaded
		output plugin. It should be ready to start doing its job.
	JE_FILTER_ADDED:
		- "plugin" : a pointer to a je_filter_plugin. This is a freshly loaded
		filter plugin. It should be ready to start doing its job.
		- "which" : an int32 specifying where the filter should go w.r.t. the
		other filters. (ie - its index in the list of filters)
	JE_MOVE_FILTER:
		- "from" : an int32 specifying the index of a filter to be moved.
		- "to" : a int32 specifying the destination index of the filter
		being moved.
	JE_GET_UI:
		- "type" : an int32 specifying the type of the plugin.
		- "which" : an int32 specifying which source or which filter.
		(only valid for sources and filters)
		Reply: (B_REPLY)
			- "view" : a pointer to a BView derived class. Might be NULL
			if things didn't go right.
	JE_LOAD_UI:
		- "view" : a pointer to a BView derived class. Might be NULL
		if things didn't go right.
	JE_UNLOAD_UI:
		- none
	JE_GET_AVAILABLE_PLUGINS:
		- none
		Reply: (JE_GET_AVAILABLE_PLUGINS)
			- "sources" : a pointer to a BMessage containing relevant info
			on all available source plugins.
			- "mixers" : a pointer to a BMessage containing relevant info
			on all available mixer plugins.
			- "filters" : a pointer to a BMessage containing relevant info
			on all available filter plugins.
			- "outputs" : a pointer to a BMessage containing relevant info
			on all available output plugins.
	JE_LOAD_PLUGIN:
		- "name" : a string specifying the directory relative name of the
		plugin image where the plugin resides.
		- "index" : an int32 specifying the index of the requested plugin.
		in the list of plugins contained in the image.
		- "type" : an int32 specifying the type of the requested plugin.
		- "which" : an int32. If the type is JE_SOURCE_PLUGIN, a 1 or 0
		specifying which source is getting the plugin. If the type is
		JE_MIXER_PLUGIN, a number between 0 and the number of filters
		currently loaded minus 1 (inclusive).
		- "window" : pointer to a BHandler derived class. More specifically a
		pointer to a window that serves as a monitor for this plugin. Only
		included if type is JE_SOURCE_PLUGIN.
	JE_UNLOAD_PLUGIN:
		In GraphicsPipeline::MessageReceived or PluginManager::MessageRecieved :
			- "plugin" : a pointer to the plugin instance being unloaded.
			- "type" : an int32 specifying the type of the plugin in "plugin"
			Reply: (B_REPLY)
				- no fields.
		In JEApp::MessageReceived :
			- "type" : an int32 specifying the type of the plugin.
			- "which" : an int32 specifying which source or which filter
			is being unloaded. (only valid for sources and filters)
	JE_PLUGIN_LOADED:
		- "desc" : a string specifying the label to give the plugin in the
		GUI.
		- "type" : an int32 specifying the type of the plugin.
		- "which" : an int32 specifying which source or which filter
	JE_SOURCE_DND:
		- "name" : a string specifying the directory relative name of the
		plugin image where the plugin resides.
		- "desc" : a string specifying the label to give the plugin in the
		GUI.
		- "index" : an int32 specifying the index of the requested plugin.
	JE_MIXER_DND:
		- "name" : a string specifying the directory relative name of the
		plugin image where the plugin resides.
		- "desc" : a string specifying the label to give the plugin in the
		GUI.
		- "index" : an int32 specifying the index of the requested plugin.
	JE_FILTER_DND:
		- "name" : a string specifying the directory relative name of the
		plugin image where the plugin resides.
		- "desc" : a string specifying the label to give the plugin in the
		GUI.
		- "index" : an int32 specifying the index of the requested plugin.
	JE_OUTPUT_DND:
		- "name" : a string specifying the directory relative name of the
		plugin image where the plugin resides.
		- "desc" : a string specifying the label to give the plugin in the
		GUI.
		- "index" : an int32 specifying the index of the requested plugin.
	JE_CONTAINER_VIEW_DND:
		- none (yet)
	
*/

#endif