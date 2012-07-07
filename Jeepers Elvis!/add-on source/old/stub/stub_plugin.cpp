/*
	stub_plugin.cpp
	Stub plugins for Jeepers Elvis!
	2000 John Wiggins
*/

#include "stub_source.h"
#include "stub_mixer.h"
#include "stub_filter.h"
#include "stub_output.h"


// the exports for this plugin
extern "C" _EXPORT je_plugin plugins[] = {
	{JE_SOURCE_PLUGIN, "Stub Source", instantiate_stub_source},
	{JE_MIXER_PLUGIN, "Stub Mixer", instantiate_stub_mixer},
	{JE_FILTER_PLUGIN, "Stub Filter 1", instantiate_stub_filter},
	{JE_FILTER_PLUGIN, "Stub Filter 2", instantiate_stub_filter},
	{JE_OUTPUT_PLUGIN, "Stub Output", instantiate_stub_output},
	{JE_NO_PLUGIN,NULL,NULL}
};