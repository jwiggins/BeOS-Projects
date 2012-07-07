/*
	JEDefaultPlugins.cpp
	Jeepers Elvis Addon comprising all packaged plugins
	2000 John Wiggins
	2002 : added fullscreen output
*/

#include "AlphaMixer/AlphaMixer.h"
#include "ChannelWhackerFilter/ChannelWhacker.h"
#include "InvertFilter/InvertFilter.h"
#include "MovieLooperSource/MovieLooper.h"
#include "FullScreenOutput/FullScreenOut.h"
//#include "JEPlugin.h"

// Alpha Mixer
je_plugin alpha_mixer = {JE_MIXER_PLUGIN, "Alpha Mixer", instantiate_alpha_mixer};
// Channel Whacker
je_plugin channel_whack = {JE_FILTER_PLUGIN, "Channel Whacker Filter", instantiate_channel_whacker_filter};
// Invert Filter
je_plugin invert_filter = {JE_FILTER_PLUGIN, "Invert Filter", instantiate_invert_filter};
// Movie Looper
je_plugin movie_looper = {JE_SOURCE_PLUGIN, "Movie Looper", movie_looper_instantiate};
// FullScreen Output
je_plugin fullscreen_output = {JE_OUTPUT_PLUGIN, "Fullscreen Output", instantiate_fullscreen_output};

// all plugins 
static je_plugin *plugins[] = {
	&movie_looper,
	&alpha_mixer,
	&invert_filter,
	&channel_whack,
	&fullscreen_output,
	NULL
};

// the plugin's exported function
extern "C" _EXPORT je_plugin **get_plugins()
{
	return plugins;
}