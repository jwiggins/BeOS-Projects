/*
	GraphicsPipeline.h
	The Heart of JE! The pipeline handles the non-UI
	related graphics kung-fu.
	2000 John Wiggins
*/

#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <app/Looper.h>
#include <app/Messenger.h>
#include <app/Message.h>
#include <app/MessageRunner.h>
#include <interface/Rect.h>
#include <interface/Bitmap.h>
#include <support/List.h>
#include <kernel/OS.h>
#include <posix/stdio.h>
#include <posix/string.h>

#include "JEPlugin.h"
#include "MessageConstants.h"

class GraphicsPipeline : public BLooper {
public:
						GraphicsPipeline();
						~GraphicsPipeline();
	
	virtual	void		MessageReceived(BMessage *msg);
	
	// Plugin returning functions
	// Allows be_app to tell us which plugins are going away
	void	*			GetSource(int32 which)
							{ return (void *)(which ? source2 : source1); };
	void	*			GetMixer()
							{ return (void *)mixer; };
	void	*			GetFilter(int32 which)
							{ return filterList.ItemAt(which); };
	void	*			GetOutput()
							{ return (void *)output; };
private:
	void				GenerateFrame();
	void				InitFrameTimer();
	void				SetFrameDuration(bigtime_t dur);
	void				HandleFramerateChange(int32 source, bigtime_t rate);
	void				AdjustOutputSize();
	void				UnloadPlugin(void *plugin, int32 type);
	void				MoveFilter(int32 from, int32 to);
	void				SyncWithSource();
	
	
	BMessageRunner		*frameTimer;
	BMessenger			*defaultOutputWindow;
	je_source_plugin	*source1, *source2;
	je_mixer_plugin		*mixer;
	je_output_plugin	*output;
	bigtime_t			frameDuration;
	bigtime_t			nextFrame;
	//bigtime_t			lastMsg;
	bigtime_t			source1Rate, source2Rate;
	bool				sourceSyncing;
	bool				source1Ready, source2Ready;
	BRect				source1Size, source2Size, outputSize;
	BBitmap				*outBits;
	BList				filterList;
	
};

#endif