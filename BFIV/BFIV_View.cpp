#include "BFIV_View.h"

BFIV_View::BFIV_View(const char *name, BFIV_Tiler *image)
: BView(image->Bounds(1.0), name, B_FOLLOW_TOP|B_FOLLOW_LEFT, B_WILL_DRAW), dirtyRegion()
{
	fatBastard = image;
	
	//image->Bounds().PrintToStream();
	
	scratchMap = new BBitmap(image->GetChunkBounds(), image->ColorSpace());
	
	if((dirtySem = create_sem(0, "dirty little bastard")) < 0)
		debugger("create_sem() failed on the \"dirty little bastard\" semaphore.");
	
	redrawThread = spawn_thread(Draw_Thread_, "the professional", B_NORMAL_PRIORITY, (void *)this);
	if(redrawThread < 0)
		debugger("spawn_thread() failed to create the redraw thread.");
	else
		resume_thread(redrawThread);
	
	SetViewColor(B_TRANSPARENT_32_BIT);
	scaleFactor = 1.0; // no resize
}

BFIV_View::~BFIV_View()
{
	delete scratchMap;
	delete fatBastard;
	
	delete_sem(dirtySem);
	// above line causes the death of our redraw thread
	// wait for it, then kill it if it's still there
	snooze(3000);
	kill_thread(redrawThread);
}

void BFIV_View::SetImageScale(float scale)
{
	BRect bounds = fatBastard->Bounds(scale); // image bounds
	
	// going to a smaller scale?
	if(scale < scaleFactor)
	{
		// exclude the extra dirty region so our drawing thread doesn't get confused
		BRegion exclude;
		exclude.Include(fatBastard->Bounds(scaleFactor)); // the region for the old scale
		exclude.Exclude(bounds); // minus the region for the new scale
		
		dirtyRegion.Exclude(&exclude); // trim from dirtyRegion
	}
	
	scaleFactor = scale; // replace scale
	ResizeTo(bounds.Width(), bounds.Height()); // resize
	Invalidate(bounds);
	Parent()->Invalidate(Parent()->Bounds());
}

void BFIV_View::Draw(BRect update)
{
	dirtyRegion.Include(update);
	
	release_sem(dirtySem);
	acquire_sem(dirtySem);
}

int32 BFIV_View::Draw_Thread_(void *data)
{
	return ((BFIV_View *)(data))->Draw_Thread();
}

int32 BFIV_View::Draw_Thread()
{
	bool keepLooping = true;
	
	while(keepLooping)
	{
		// DO THIS FIRST
		if( acquire_sem(dirtySem) == B_OK )
		{
			release_sem(dirtySem); // the view gets it back immediately
			// DO THIS FIRST
			
			while(dirtyRegion.CountRects() > 0) // keep going until it's clean
			{
				for(uint32 i=0; i < fatBastard->CountChunks(); i++)
				{
					BRect dRect = fatBastard->RectForChunk(i), sRect;
					
					sRect = dRect; // source rect doesn't get scaled
					dRect = fatBastard->ScaleRect(dRect, i, scaleFactor); // scale dest
					
					// do we need to draw this one?
					if(dirtyRegion.Intersects(dRect))
					{
						fatBastard->GetImageChunk(i, scratchMap); // grab the chunk
						
						sRect.OffsetTo(0,0); // source's top corner must be (0,0)
						
						if(LockLooper()) // Lock!
						{
							DrawBitmapAsync(scratchMap, sRect, dRect); // draw
							//StrokeRect(dRect, B_SOLID_LOW); // debug only
							Sync(); // sync
							
							dirtyRegion.Exclude(dRect); // we've done this one....
							UnlockLooper(); // Unlock!
						}
					}
				}
			}
		}
		else
			keepLooping = false; // natural death when dirtySem is deleted
	}
	
	return B_OK;
}