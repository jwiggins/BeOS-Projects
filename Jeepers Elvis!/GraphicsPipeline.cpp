/*
	GraphicsPipeline.cpp
	2000 John Wiggins
*/

#include "GraphicsPipeline.h"

GraphicsPipeline::GraphicsPipeline()
: BLooper("Graphics Pipeline", B_DISPLAY_PRIORITY),
 filterList(10)
{	
	//source frame rates
	source1Rate = 0;
	source2Rate = 0;
	
	// next frame in 5... 4... 3... ... ...
	nextFrame = 0; // not yet :P
	
	// our output bitmap
	outBits = NULL;
	
	// source synchronization flag
	sourceSyncing = false;
	
	// source readyness flags
	source1Ready = false;
	source2Ready = false;
	
	// init our happy little plugin pointers
	source1 = NULL;
	source2 = NULL;
	mixer = NULL;
	output = NULL;
	
	// messenger to our (default) output window
	defaultOutputWindow = NULL;
	
	// set our frame duration
	frameDuration = 1000 * 1000; // 1 sec per frame... (we aren't making frames yet :)
	
	// and we ain't got a frame timer yet
	frameTimer = NULL;
	
	// remind yourself to init the frame timer
	PostMessage(JE_INIT_TIMER);
	
	// debug
	//lastMsg = system_time();
}

GraphicsPipeline::~GraphicsPipeline()
{
	if (frameTimer != NULL)
		delete frameTimer;
	if (defaultOutputWindow != NULL)
		delete defaultOutputWindow;
	if (outBits != NULL)
		delete outBits;
}

void GraphicsPipeline::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case JE_GENERATE_FRAME:
		{
			//printf("GraphicsPipeline::MessageReceived(JE_GENERATE_FRAME)\n");
			//printf("JE_GENERATE_FRAME : delta = %Ld \n", system_time() - lastMsg);
			//lastMsg = system_time();
			
			GenerateFrame();
			break;
		}
		case JE_FRAMERATE_CHANGED:
		{
			//printf("GraphicsPipeline::MessageReceived(JE_FRAMERATE_CHANGED)\n");
			// a source's framerate is being reported
			// adjust our framerate as needed
			bigtime_t rate;
			int32 which;
			
			if (msg->FindInt64("framerate", &rate) == B_OK)
			{
				if (msg->FindInt32("which", &which) == B_OK)
				{
					// handle the framerate change
					HandleFramerateChange(which, rate);
					// sync up with the fastest source
					SyncWithSource();
				}
			}
			
			break;
		}
		case JE_FRAME_SIZE_CHANGED:
		{
			//printf("GraphicsPipeline::MessageReceived(JE_FRAME_SIZE_CHANGED)\n");
			// one of those zany sources changed the size of its output
			// warn the mixer and let it sort out the details
			int32 which;
			
			if (msg->FindInt32("which", &which) == B_OK)
			{
				if (which == 0)
					source1Size = source1->outputSize(source1);
				else
					source2Size = source2->outputSize(source2);
				
				AdjustOutputSize();
			}
			
			break;
		}
		case JE_SOURCE_READY:
		{
			//printf("GraphicsPipeline::MessageReceived(JE_SOURCE_READY)\n");
			// a source is ready to play
			int32 which;
			
			if (msg->FindInt32("which", &which) == B_OK)
			{
				if (which == 0)
					source1Ready = true;
				else
					source2Ready = true;
				
				// sync up with the fastest source
				SyncWithSource();
			}
			
			break;
		}
		case JE_SOURCE_CHANGED:
		{
			//printf("GraphicsPipeline::MessageReceived(JE_SOURCE_CHANGED)\n");
			// a source plugin has changed
			// cache a pointer and wait for the ready msg
			void *plugin;
			int32 which;
			
			if (msg->FindPointer("plugin", &plugin) == B_OK)
			{
				if (msg->FindInt32("which", &which) == B_OK)
				{
					if (which == 0)
						source1 = (je_source_plugin *)plugin;
					else
						source2 = (je_source_plugin *)plugin;
				}
			}
			
			break;
		}
		case JE_MIXER_CHANGED:
		{
			//printf("GraphicsPipeline::MessageReceived(JE_MIXER_CHANGED)\n");
			// the mixer plugin changed
			// cache a pointer and adjust the output size if needed
			void *plugin;
			
			if (msg->FindPointer("plugin", &plugin) == B_OK)
				mixer = (je_mixer_plugin *)plugin;
			
			// if a source is ready, reset the frame timer
			if (source1Ready || source2Ready)
			{
				if (frameTimer != NULL)
					frameTimer->SetInterval(frameDuration); // frame rate
			}
			
			// a new mixer may mean a new output size
			// call AdjustOutputSize to keep things current
			AdjustOutputSize();
			
			break;
		}
		case JE_OUTPUT_CHANGED:
		{
			//printf("GraphicsPipeline::MessageReceived(JE_OUTPUT_CHANGED)\n");
			// the output plugin changed
			// cache a pointer and continue business as usual
			void *plugin;
			
			if (msg->FindPointer("plugin", &plugin) == B_OK)
				output = (je_output_plugin *)plugin;
			
			break;
		}
		case JE_FILTER_ADDED:
		{
			//printf("GraphicsPipeline::MessageReceived(JE_FILTER_ADDED)\n");
			// a filter plugin was added
			// put it in the list at the specified index and continue chugging along
			void *plugin;
			int32 which;
			
			if (msg->FindPointer("plugin", &plugin) == B_OK)
			{
				if (msg->FindInt32("which", &which) == B_OK)
				{
					// if it fails to insert at the specified index
					// then just insert it at the end
					if (!filterList.AddItem(plugin, which))
						filterList.AddItem(plugin);
				}
			}
			
			break;
		}
		case JE_MOVE_FILTER:
		{
			//printf("GraphicsPipeline::MessageReceived(JE_MOVE_FILTER)\n");
			// "move" a filter
			// change the order of the filters
			int32 from, to;
			
			if (msg->FindInt32("from", &from) == B_OK)
			{
				if (msg->FindInt32("to", &to) == B_OK)
				{
					MoveFilter(from, to);
				}
			}
			break;
		}
		case JE_UNLOAD_PLUGIN:
		{
			//printf("GraphicsPipeline::MessageReceived(JE_UNLOAD_PLUGIN)\n");
			// a plugin is being unloaded
			// stop using said plugin and reply
			void *plugin;
			int32 type;
			
			if (msg->FindPointer("plugin", &plugin) == B_OK)
			{
				if (msg->FindInt32("type", &type) == B_OK)
				{
					UnloadPlugin(plugin, type);
				}
			}
			
			// we MUST reply to this message no matter what
			// the plugin manager is blocked until we do
			msg->SendReply(B_REPLY); // simple reply
			
			break;
		}
		case JE_SET_DEFAULT_OUTPUT:
		{
			//printf("GraphicsPipeline::MessageReceived(JE_SET_DEFAULT_OUTPUT)\n");
			// we're getting a pointer to our default output window
			// construct a BMessenger to it
			void *window;
			
			if (msg->FindPointer("window", &window) == B_OK)
			{
				// default output
				defaultOutputWindow = new BMessenger(reinterpret_cast<BHandler *>(window));
				if (!defaultOutputWindow->IsValid())
				{
					delete defaultOutputWindow;
					defaultOutputWindow = NULL;
				}
			}
			
			break;
		}
		case JE_UNSET_DEFAULT_OUTPUT:
		{
			// get rid of the defaultOutputWindow messenger
			if (defaultOutputWindow != NULL)
			{
				delete defaultOutputWindow;
				defaultOutputWindow = NULL;
			}
			break;
		}
		case JE_INIT_TIMER:
		{
			InitFrameTimer();
			break;
		}
		default:
		{
			BLooper::MessageReceived(msg);
			break;
		}
	}
}

// the big momma of the lot
// GenerateFrame is our sole purpose
// - generates frames and outputs them
// - drops frames and re-adjusts framerate when needed
// - syncs with sources (second half of sync process)
void GraphicsPipeline::GenerateFrame()
{
	// check the sync flag
	if (sourceSyncing)
	{
		//printf("GraphicsPipeline::GenerateFrame() : source sync\n");
		// currently syncing with a source
		// reset our frame timer
		if (frameTimer)
		{
			// restore the framerate
			frameTimer->SetInterval(frameDuration);
			// make sure we keep getting messages
			//frameTimer->SetCount(-1); // -1 means infinite
		}
		
		// make sure to set nextFrame
		nextFrame = system_time() + frameDuration;
		//printf("GraphicsPipeline::GenerateFrame() : next at %Ld \n", nextFrame);
		// and to un-set the sync flag
		sourceSyncing = false;
	}
	else if ((source1Ready || source2Ready) && mixer != NULL)
	{
		//printf("GraphicsPipeline::GenerateFrame() : frame\n");
		// one source is ready and we have a mixer
		bigtime_t start = system_time(), finish;
		
		// check to see if this message arrived on time
		if (start > (5000 + nextFrame))
		{
			//printf("GraphicsPipeline::GenerateFrame() : late!\n");
			// we need to drop this frame
			nextFrame = start + frameDuration;
		}
		else // we're on time (yay)
		{
			// first, we mix two sources
			if (source1Ready && source2Ready) // both sources
			{
				//printf("GraphicsPipeline::GenerateFrame() : both sources\n");
				// lock the sources
				source1->lock(source1);
				source2->lock(source2);
				// mix
				mixer->mix(mixer, source1->getFrame(source1), source2->getFrame(source2), outBits);
				// unlock the sources
				source1->unlock(source1);
				source2->unlock(source2);
			}
			else if (source1Ready) // source 1
			{
				//printf("GraphicsPipeline::GenerateFrame() : first source\n");
				// lock the source
				source1->lock(source1);
				// mix
				mixer->mix(mixer, source1->getFrame(source1), NULL, outBits);
				// unlock the source
				source1->unlock(source1);
			}
			else // source 2
			{
				//printf("GraphicsPipeline::GenerateFrame() : second source\n");
				// lock the source
				source2->lock(source2);
				// mix
				mixer->mix(mixer, source2->getFrame(source2), NULL, outBits);
				// unlock the source
				source2->unlock(source2);
			}
			
			// then, filter to perfection
			je_filter_plugin *filter;
			int32 i=0;
			
			while ((filter = (je_filter_plugin *)filterList.ItemAt(i)) != NULL)
			{
				//printf("GraphicsPipeline::GenerateFrame() : filter\n");
				// filter
				filter->filter(filter, outBits);
				
				i++; // next filter
			}
			
			// now, output!
			if (output)
			{
				//printf("GraphicsPipeline::GenerateFrame() : output plugin\n");
				output->output(output, outBits);
			}
			
			// other output goes here
			
			// this is the default output
			// we send a pointer to the frame's bitmap
			// the window will draw as soon as it gets the pointer
			// we don't worry about locking, because the output plugin's output
			// is the only output that really matters
			if (defaultOutputWindow != NULL)
			{
				//printf("GraphicsPipeline::GenerateFrame() : default output\n");
				BMessage msg(JE_DRAW_BITMAP);
				msg.AddPointer("bitmap", outBits);
				defaultOutputWindow->SendMessage(&msg);
			}
			
			
			// prepare for the next frame
			// adjust framerate if needed
			// note the finish time
			finish = system_time();
			
			// check to see if we took too long
			if ((finish - start) > frameDuration)
			{
				// oh shit... time to decrease the framerate
				bigtime_t newRate = (finish - start) + 1000;
				
				SetFrameDuration((newRate/1000)*1000 + (((newRate % 1000) > 500) ? 1000 : 0));
			}
			
			// next frame at ...
			//nextFrame = finish + (frameDuration - (finish - start));
			nextFrame = start + frameDuration;
			//printf("GraphicsPipeline::GenerateFrame() : next at %Ld \n", nextFrame);
		}
	}
}

void GraphicsPipeline::InitFrameTimer()
{
	// set up the BMessageRunner frame timer thingy :P
	// leave it NULL if construction fails
	status_t err;
	frameTimer = new BMessageRunner(BMessenger(this),
					new BMessage(JE_GENERATE_FRAME), frameDuration);
	if ((err = frameTimer->InitCheck()) != B_OK)
	{
		//printf("GraphicsPipeline::InitFrameTimer() : bad frame timer\n");
		//printf("GraphicsPipeline::InitFrameTimer() : err = %s\n", strerror(err));
		delete frameTimer;
		frameTimer = NULL;
	}
	
	// set our frame rate
	SetFrameDuration(1000 * 1000); // 33ms per frame =  ~30 fps
									// 2002 : Except when it's 1s per frame = 1 fps
}

void GraphicsPipeline::SetFrameDuration(bigtime_t dur)
{
	frameDuration = dur;
	
	if (frameTimer != NULL)
		frameTimer->SetInterval(frameDuration);
}

void GraphicsPipeline::HandleFramerateChange(int32 source, bigtime_t rate)
{
	// cache the value
	if (source == 0)
		source1Rate = rate;
	else
		source2Rate = rate;
	
	// then decide what the final output rate should be
	// change if the new rate is faster than the current
	if (frameDuration > rate)
		SetFrameDuration((rate/1000)*1000 + (((rate % 1000) > 500) ? 1000 : 0));
		// funky math above just rounds to a nice even msec value
		// (since duration is in usecs)
}

void GraphicsPipeline::AdjustOutputSize()
{
	BRect mixOutSize;
	
	// only if we have a mixer and at least one valid source
	if (mixer && (source1Size.IsValid() || source2Size.IsValid()))
	{
		// first get the mixer to tell us what it wants
		mixOutSize = mixer->getOutputSize(mixer, source1Size, source2Size);
		
		// compare that to what it wanted before
		if (mixOutSize != outputSize)
		{
			// change our size
			outputSize = mixOutSize;
			
			// now change the two output bitmaps
			if (outBits)
				delete outBits;
			
			// re-construct
			outBits = new BBitmap(outputSize, B_RGB32);
		}
	}
}

void GraphicsPipeline::UnloadPlugin(void *plugin, int32 type)
{
	// "unloading" is fairly simplistic
	// all we do is NULLify our pointers to said plugin
	switch(type)
	{
		case JE_SOURCE_PLUGIN:
		{
			// source unload is a little more complex
			// - NULLify pointer
			// - invalidate size rect
			// - unset the readyness flag
			if (plugin == source1)
			{
				source1 = NULL;
				source1Size = BRect();
				source1Ready = false;
			}
			else if (plugin == source2)
			{
				source2 = NULL;
				source2Size = BRect();
				source2Ready = false;
			}
			
			// reset the timer if both sources are gone
			if (!source1Ready && !source2Ready)
			{
				if (frameTimer != NULL)
					frameTimer->SetInterval(1000 * 1000); // 1 sec
			}
			
			break;
		}
		case JE_MIXER_PLUGIN:
		{
			mixer = NULL;
			
			// reset the timer when the mixer goes too
			if (frameTimer != NULL)
				frameTimer->SetInterval(1000 * 1000); // 1 sec
			
			break;
		}
		case JE_FILTER_PLUGIN:
		{
			int32 i=0;
			void *filter = NULL;
			
			while ((filter = filterList.ItemAt(i)) != NULL)
			{
				if (filter == plugin)
				{
					filter = filterList.RemoveItem(i);
					break;
				}
				i++;
			}
			
			break;
		}
		case JE_OUTPUT_PLUGIN:
		{
			output = NULL;
			break;
		}
		default:
		{
			break;
		}
	}
}

void GraphicsPipeline::MoveFilter(int32 from, int32 to)
{
	//printf("GraphicsPipeline::MoveFilter(from = %ld, to = %ld)\n", from, to);
	filterList.MoveItem(from, to);
}

void GraphicsPipeline::SyncWithSource()
{
	//printf("GraphicsPipeline::SyncWithSource()\n");
	// Mmmmmm nasty synchronization with sources
	bigtime_t wakeup_time;
	
	if (source1 != NULL && source2 != NULL)
	{
		//printf("GraphicsPipeline::SyncWithSource() : both sources\n");
		// when should we wake up?
		// pick the fastest source
		if (source1Rate < source2Rate)
			wakeup_time = source1->nextFrameTime(source1);
		else
			wakeup_time = source2->nextFrameTime(source2);
		
		// add fudge factor
		wakeup_time += 1000;
		// then adjust the timer 
		if (frameTimer != NULL)
		{
			//printf("GraphicsPipeline::SyncWithSource() : setting timer\n");
			frameTimer->SetInterval(wakeup_time - system_time());
			//frameTimer->SetCount(1); // only once 
		}
		// set the flag, and we're done
		sourceSyncing = true;
	}
	else if (source1 != NULL)
	{
		//printf("GraphicsPipeline::SyncWithSource() : first source\n");
		// wakeup time
		wakeup_time = source1->nextFrameTime(source1);
		// add fudge factor
		wakeup_time += 1000;
		// then adjust the timer 
		if (frameTimer != NULL)
		{
			//printf("GraphicsPipeline::SyncWithSource() : setting timer\n");
			frameTimer->SetInterval(wakeup_time - system_time());
			//frameTimer->SetCount(1); // only once 
		}
		// set the flag, and we're done
		sourceSyncing = true;
	}
	else if (source2 != NULL)
	{
		//printf("GraphicsPipeline::SyncWithSource() : second source\n");
		// wakeup time
		wakeup_time = source2->nextFrameTime(source2);
		// add fudge factor
		wakeup_time += 1000;
		// then adjust the timer 
		if (frameTimer != NULL)
		{
			//printf("GraphicsPipeline::SyncWithSource() : setting timer\n");
			frameTimer->SetInterval(wakeup_time - system_time());
			//frameTimer->SetCount(1); // only once 
		}
		// set the flag, and we're done
		sourceSyncing = true;
	}
}
