/*
	VideoReader.cpp
	2000 John Wiggins
*/

#include "VideoReader.h"

VideoReader::VideoReader()
: clipList(10), mutexLock(true)
{
	// -1 == no clip
	currentClipIndex = -1;
	currentFramerate = 0;
	nextFrameTime = 0;
	currentFrameNum = 0;
	playDirection = DIR_FORWARDS;
	frameDelta = 0;
	clipLengthFrames = 0;
	clipLengthTime = 0;
	width = 0.0;
	height = 0.0;
	
	// no clip yet
	currentClip = NULL;
	// no open file yet
	clipFile = NULL;
	// definately no video track yet
	videoTrack = NULL;
	// no ui yet
	uiMessenger = NULL;
}

VideoReader::~VideoReader()
{
	// empty the clip list
	video_clip *clip = NULL;
	while ((clip = (video_clip *)clipList.RemoveItem(0L)) != NULL)
		delete clip;
	
	if (clipFile != NULL)
		delete clipFile;
	if (uiMessenger != NULL)
		delete uiMessenger;
}

status_t VideoReader::SniffFile(entry_ref *ref)
{
	status_t err = B_ERROR;
	bool vidTrackFound = false;
	BMediaFile *mediaFile;
	BMediaTrack *mediaTrack = NULL;
	
	// make a BMediaFile
	mediaFile = new BMediaFile(ref);
	// check it
	if ((err = mediaFile->InitCheck()) == B_OK)
	{
		int32 numTracks = mediaFile->CountTracks();
		for (int32 i=0; i < numTracks; i++)
		{
			// get a media track
			mediaTrack = mediaFile->TrackAt(i);
			
			if (mediaTrack != NULL && (err = mediaTrack->InitCheck()) == B_OK)
			{
				media_format mf;
				
				mediaTrack->EncodedFormat(&mf);
				if (mf.type == B_MEDIA_ENCODED_VIDEO)
				{
					// ok, we found a video track
					// now we check to see if the decoder
					// will give us B_RGB32 output
					mf.u.raw_video = media_raw_video_format::wildcard;
					mf.u.raw_video.display.format = B_RGB32;
					mediaTrack->DecodedFormat(&mf);
					
					// only say we found a track if it has
					// right output qualities (namely 32bpp)
					if (mf.u.raw_video.display.format == B_RGB32)
						vidTrackFound = true;
					
					// we found the video
					// release the track and bail
					mediaFile->ReleaseTrack(mediaTrack);
					break;
				}
				
				// release the track
				mediaFile->ReleaseTrack(mediaTrack);
			}
		}
		
		// return an error if there wasn't a video track
		if (!vidTrackFound)
			err = B_ERROR;
	}
	
	// delete the media file
	delete mediaFile;
	
	return err;
}

status_t VideoReader::LoadFile(entry_ref *ref)
{
	// load a clip into the list
	status_t err;
	video_clip *clip = new video_clip;
	
	clip->ref = *ref;
	
	clip->start = 0;
	clip->last = 0;
	clip->direction = DIR_FORWARDS;
	
	// now find out the frame rate, frame count and video track
	BMediaFile *mediaFile;
	BMediaTrack *mediaTrack = NULL;
	
	// open the file
	mediaFile = new BMediaFile(&clip->ref);
	
	// find the video track
	if ((err = mediaFile->InitCheck()) == B_OK)
	{
		int32 numTracks = mediaFile->CountTracks();
		for (int32 i=0; i < numTracks; i++)
		{
			// get a media track
			mediaTrack = mediaFile->TrackAt(i);
			
			if (mediaTrack != NULL && (err = mediaTrack->InitCheck()) == B_OK)
			{
				media_format mf;
				
				mediaTrack->EncodedFormat(&mf);
				if (mf.type == B_MEDIA_ENCODED_VIDEO)
				{
					// ok, we found the video track
					clip->vidtrack = i;
					clip->stop = mediaTrack->CountFrames();
					clip->framerate = mediaTrack->Duration()/clip->stop;
					
					// add the clip to the clipList
					clipList.AddItem((void *)clip);
					
					// we found the video
					// release the track and bail
					mediaFile->ReleaseTrack(mediaTrack);
					break;
				}
				
				// release the track
				mediaFile->ReleaseTrack(mediaTrack);
			}
		}
	}
	
	// get rid of the mediaFile
	delete mediaFile;
	
	if (err != B_OK)
	{
		// couldn't properly init the clip struct
		// get rid of it and chalk one up to the bit gods
		delete clip;
	}
	
	return err;
}

status_t VideoReader::UnloadFile(int32 index)
{
	if (!mutexLock.IsLocked())
		debugger("VideoReader must be locked before unloading a file\n");
	
	status_t err = B_ERROR;
	
	// don't unload the current clip (Bad Thing [tm])
	if (index != currentClipIndex)
	{
		video_clip *clip = (video_clip *)clipList.RemoveItem(index);
		
		if (clip != NULL)
		{
			// adjust the current clip index (if needed)
			if (currentClipIndex > index)
				currentClipIndex--;
			// clean up
			delete clip;
			// indicate success
			err = B_OK;
		}
	}
	
	return err;
}

status_t VideoReader::LoadClipForPlaying(int32 index, BBitmap **bitmap, bool seek)
{
	if (!mutexLock.IsLocked())
		debugger("VideoReader must be locked before loading clip\n");
	
	status_t err = B_ERROR;
	video_clip *clip;
	
	// first unload the previous clip if it exists
	if (clipFile != NULL)
		UnloadClip();
	
	// now load the file and init the video track
	clip = (video_clip *)clipList.ItemAt(index);
	if (clip != NULL)
	{
		// open the file
		clipFile = new BMediaFile(&clip->ref);
		
		// get the video track
		if ((err = clipFile->InitCheck()) == B_OK)
		{
			videoTrack = clipFile->TrackAt(clip->vidtrack);
			
			// validate returned track	
			if (videoTrack != NULL && (err = videoTrack->InitCheck()) == B_OK)
			{
				// setup the frame bitmap
				media_format mf;
				if ((err = videoTrack->EncodedFormat(&mf)) == B_OK)
				{
					BRect bounds(0.0,
								0.0,
								mf.u.encoded_video.output.display.line_width - 1.0,
								mf.u.encoded_video.output.display.line_count - 1.0);
					
					// trash the old frame bitmap if it's there
					if (*bitmap != NULL)
					{
						delete *bitmap;
						*bitmap = NULL;
					}
					
					// build a new frame bitmap
					*bitmap = new BBitmap(bounds, B_RGB32);
					
					// set up a proper decoded video format
					memset(&mf, 0, sizeof(media_format));
					mf.u.raw_video.last_active = bounds.IntegerHeight()-1;
					mf.u.raw_video.orientation = B_VIDEO_TOP_LEFT_RIGHT;
					mf.u.raw_video.pixel_width_aspect = 1; //ignored?
					mf.u.raw_video.pixel_height_aspect = 1; // ignored?
					mf.u.raw_video.display.format = (*bitmap)->ColorSpace();
					mf.u.raw_video.display.line_width = bounds.IntegerWidth();
					mf.u.raw_video.display.line_count = bounds.IntegerHeight();
					mf.u.raw_video.display.bytes_per_row = (*bitmap)->BytesPerRow();
					
					// make sure everything is cool with that format
					if ((err = videoTrack->DecodedFormat(&mf)) == B_OK)
					{
						// seek to the last frame
						if (seek)
						{
							// set currentFrameNum
							// videoTrack->SeekToFrame() will adjust it
							currentFrameNum = clip->last;
							switch(clip->direction)
							{
								case DIR_BACKWARDS:
									videoTrack->SeekToFrame(&currentFrameNum, B_MEDIA_SEEK_CLOSEST_BACKWARD);
									break;
								case DIR_FORWARDS:
								case DIR_BACK_AND_FORTH:
								default:
									videoTrack->SeekToFrame(&currentFrameNum, B_MEDIA_SEEK_CLOSEST_FORWARD);
									break;
							}
						}
						else // no seek
						{
							switch(clip->direction)
							{
								case DIR_BACKWARDS:
									currentFrameNum = clip->stop;
									videoTrack->SeekToFrame(&currentFrameNum, B_MEDIA_SEEK_CLOSEST_BACKWARD);
									break;
								case DIR_FORWARDS:
								case DIR_BACK_AND_FORTH:
								default:
									currentFrameNum = clip->start;
									videoTrack->SeekToFrame(&currentFrameNum, B_MEDIA_SEEK_CLOSEST_FORWARD);
									break;
							}
						}
						
						// set up other clip state		
						currentClipIndex = index;
						currentFramerate = clip->framerate;
						clipLengthFrames = videoTrack->CountFrames();
						clipLengthTime = videoTrack->Duration();
						width = bounds.Width();
						height = bounds.Height();
						playDirection = clip->direction;
						if (playDirection == DIR_BACKWARDS)
							frameDelta = -1;
						else
							frameDelta = 1;
						nextFrameTime = system_time() + currentFramerate; // a bit arbitrary
						
						// it's now official
						currentClip = clip;
					}
					else
					{
						// nope!
						delete *bitmap;
						*bitmap = NULL;
					}
				}
			}
			else
			{
				err = B_ERROR;
			}
		}
		
		// bad init, clean up
		if (err != B_OK)
		{
			delete clipFile;
			clipFile = NULL;
			videoTrack = NULL;
			currentClip = NULL;
		}
	}
	
	return err;
}

void VideoReader::UnloadClip()
{
	if (!mutexLock.IsLocked())
		debugger("VideoReader must be locked before unloading clip\n");
	
	// close the video file
	if (clipFile != NULL && videoTrack != NULL && currentClip != NULL)
	{
		// save track state into clip
		currentClip->last = videoTrack->CurrentFrame();
		currentClip->direction = playDirection;
		
		// clean up
		clipFile->ReleaseTrack(videoTrack);
		delete clipFile;
		
		videoTrack = NULL;
		clipFile = NULL;
		currentClip = NULL;
	}
	
	// reset state
	currentClipIndex = -1;
	currentFramerate = 0;
	nextFrameTime = 0;
	currentFrameNum = 0;
	clipLengthFrames = 0;
	clipLengthTime = 0;
	frameDelta = 0;
	width = 0.0;
	height = 0.0;
	playDirection = DIR_FORWARDS;
}

int32 VideoReader::CurrentClip() const
{
	return currentClipIndex;
}

void VideoReader::GetClipDimensions(float *w, float *h) const
{
	*w = width;
	*h = height;
}

int64 VideoReader::CurrentFrameNumber() const
{
	return currentFrameNum;
}

void VideoReader::GetNextFrame(BBitmap *bits)
{
	if (!mutexLock.IsLocked())
		debugger("VideoReader must be locked before grabbing a frame\n");
	
	if (clipFile != NULL && videoTrack != NULL && currentClip != NULL)
	{
		int64 framesRead;
		
		// first, set the next frame time
		nextFrameTime = system_time() + currentFramerate;
		
		// step
		currentFrameNum += frameDelta;
		
		//printf("VideoReader::GetNextFrame() : currentFrameNum = %Ld\n", currentFrameNum);
		
		// negative frame delta means we need to seek before we
		// grab the frame data
		if (frameDelta < 0)
		{
			// don't let it fuck with our frame number
			int64 seekFrame = currentFrameNum;
			status_t err;
			err = videoTrack->SeekToFrame(&seekFrame, B_MEDIA_SEEK_CLOSEST_BACKWARD);
			//printf("VideoReader::GetNextFrame() : seek error = %s\n", strerror(err));
		}
		
		// get the frame
		videoTrack->ReadFrames(bits->Bits(), &framesRead);
		
		// and get ready for the next frame
		switch(playDirection)
		{
			case DIR_FORWARDS:
			{
				// if we've reached the end, start over
				if (currentFrameNum >= currentClip->stop)
				{
					currentFrameNum = currentClip->start;
					
					// then seek to where we need to go
					int64 seekFrame = currentFrameNum;
					videoTrack->SeekToFrame(&seekFrame, B_MEDIA_SEEK_CLOSEST_FORWARD);
				}
				break;
			}
			case DIR_BACKWARDS:
			{
				// if we've reached the beginning, start over
				if (currentFrameNum <= currentClip->start)
				{
					currentFrameNum = currentClip->stop;
					
					// then seek to where we need to go
					int64 seekFrame = currentFrameNum;
					videoTrack->SeekToFrame(&seekFrame, B_MEDIA_SEEK_CLOSEST_BACKWARD);
				}
				break;
			}
			case DIR_BACK_AND_FORTH:
			{
				// switch directions when needed
				if (frameDelta < 0 && currentFrameNum <= currentClip->start)
					frameDelta = -frameDelta;
				else if (frameDelta > 0 && currentFrameNum >= currentClip->stop)
					frameDelta = -frameDelta;
				
				break;
			}
			default:
				break;
		}
		
		// tell the ui view where we're at
		if (uiMessenger != NULL)
		{
			BMessage msg(ML_SET_INDICATOR_POSITION);
			msg.AddInt64("where", currentFrameNum);
			uiMessenger->SendMessage(&msg);
		}
	}
}

int64 VideoReader::ClipStart() const
{
	if (currentClip != NULL)
		return currentClip->start;
	else
		return 0;
}

int64 VideoReader::ClipEnd() const
{
	if (currentClip != NULL)
		return currentClip->stop;
	else
		return 0;
}

clip_direction VideoReader::ClipDirection() const
{
	if (currentClip != NULL)
		return currentClip->direction;
	else
		return DIR_FORWARDS;
}

void VideoReader::SetClipStart(int64 frame)
{
	if (!mutexLock.IsLocked())
		debugger("VideoReader must be locked before setting a start point\n");
	
	if (currentClip != NULL)
	{
		if (frame < currentClip->stop)
			currentClip->start = frame;
		else // fudge factor state
			currentClip->start = currentClip->stop - 1;
		
		// "seek" to safe spot if needed
		if (currentFrameNum < currentClip->start)
			currentFrameNum = currentClip->start + 1;
	}
}

void VideoReader::SetClipEnd(int64 frame)
{
	if (!mutexLock.IsLocked())
		debugger("VideoReader must be locked before setting an end point\n");
	
	if (currentClip != NULL)
	{
		if (frame > currentClip->start)
			currentClip->stop = frame;
		else // fudge factor state
			currentClip->stop = currentClip->start + 1;
		
		// "seek" to safe spot if needed
		if (currentFrameNum > currentClip->stop)
			currentFrameNum = currentClip->stop - 1;
	}
}

void VideoReader::SetClipDirection(clip_direction dir)
{
	if (!mutexLock.IsLocked())
		debugger("VideoReader must be locked before a clip\'s direction\n");
	
	if (currentClip != NULL)
	{
		currentClip->direction = dir;
		playDirection = dir;
		
		if (dir == DIR_FORWARDS)
		{
			frameDelta = 1;
			if (currentFrameNum >= currentClip->stop)
				currentFrameNum = currentClip->start;
		}
		else if (dir == DIR_BACKWARDS)
		{
			frameDelta = -1;
			if (currentFrameNum <= currentClip->start)
				currentFrameNum = currentClip->stop;
		}
		else
		{
			frameDelta = 1; // default to forwards on DIR_BACK_AND_FORTH
			if (currentFrameNum >= currentClip->stop)
				currentFrameNum = currentClip->start;
		}
	}
}

void VideoReader::SetUIMessenger(BView *view)
{
	uiMessenger = new BMessenger(view);
}
