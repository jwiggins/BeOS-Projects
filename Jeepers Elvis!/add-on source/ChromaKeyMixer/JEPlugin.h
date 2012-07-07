/*
	JEPlugin.h
	All struct definitions for Jeepers Elvis! plugins
	2000 John Wiggins
*/

#ifndef JE_PLUGIN_H
#define JE_PLUGIN_H

#include <support/SupportDefs.h>
#include "PluginMessages.h"

/*
	Example:
	
void * instantiate_example_source(void *arg);
void * instantiate_example_mixer(void *arg);
void * instantiate_example_filter(void *arg);

extern "C" _EXPORT je_plugin **get_plugins();

je_plugin source_plugin =
	{JE_SOURCE_PLUGIN, "Example Source Plugin", instantiate_example_source};
je_plugin mixer_plugin =
	{JE_MIXER_PLUGIN, "Example Mixer Plugin", instantiate_example_mixer};
je_plugin filter_plugin =
	{JE_FILTER_PLUGIN, "Example Filter Plugin", instantiate_example_filter};

static je_plugin *plugins[] = {
	&source_plugin,
	&mixer_plugin,
	&filter_plugin,
	NULL
};

// exported function
je_plugin **get_plugins()
{
	return plugins;
}

*/

class BView;
class BBitmap;
class BRect;
class BMessenger;

enum je_plugin_type {
	JE_SOURCE_PLUGIN = 1,
	JE_MIXER_PLUGIN,
	JE_FILTER_PLUGIN,
	JE_OUTPUT_PLUGIN
};

// generic struct for all plugins
struct je_plugin {
	je_plugin_type type;
	const char *short_desc; // 63 chars or less please
 	void *(*instantiate)(void *); // void * instantiate(void *arg);
};

// a source plugin
struct je_source_plugin {
	void (*start)(je_source_plugin *cookie);
	void (*stop)(je_source_plugin *cookie);
	void (*run)(je_source_plugin *cookie);
	void (*exit)(je_source_plugin *cookie);
	void (*free)(je_source_plugin *cookie);
	bool (*lock)(je_source_plugin *cookie);
	void (*unlock)(je_source_plugin *cookie);
	BView *(*getUI)(je_source_plugin *cookie);
	BBitmap *(*getFrame)(je_source_plugin *cookie);
	BRect (*outputSize)(je_source_plugin *cookie);
	bigtime_t (*nextFrameTime)(je_source_plugin *cookie);
	
	void *data;
};

// args passed to a source instantiation function
// Both BMessengers must be delete'd by the plugin when it is free()d
struct je_source_args {
	BMessenger *graphics_pipeline;
	BMessenger *window;
	int32 id;
};

// a mixer plugin
struct je_mixer_plugin {
	void (*mix)(je_mixer_plugin *cookie, BBitmap *src1, BBitmap *src2, BBitmap *dst);
	void (*free)(je_mixer_plugin *cookie);
	BRect (*getOutputSize)(je_mixer_plugin *cookie, BRect src1, BRect src2);
	BView *(*getUI)(je_mixer_plugin *cookie);
	
	void *data;
};

// no args for mixer instantiation

// a filter plugin
struct je_filter_plugin {
	void (*filter)(je_filter_plugin *cookie, BBitmap *frame);
	void (*free)(je_filter_plugin *cookie);
	BView *(*getUI)(je_filter_plugin *cookie);
	
	void *data;
};

// no args for filter instantiation

// an output plugin
struct je_output_plugin {
	void (*output)(je_output_plugin *cookie, BBitmap *frame);
	void (*free)(je_output_plugin *cookie);
	BView *(*getUI)(je_output_plugin *cookie);
	
	void *data;
};

// no args for output instantiation

#endif