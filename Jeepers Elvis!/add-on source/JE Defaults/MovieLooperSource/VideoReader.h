/*
	VideoReader.h
	VideoReader class for Movie Looper (a Jeepers Elvis plugin)
	2000 John Wiggins
*/

#ifndef VIDEO_READER_H
#define VIDEO_READER_H

#include <media/MediaFile.h>
#include <media/MediaTrack.h>
#include <interface/View.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <interface/GraphicsDefs.h>
#include <storage/File.h>
#include <storage/Entry.h>
#include <support/List.h>
#include <support/Locker.h>
#include <posix/stdio.h>
#include <posix/string.h>

#include "MLMessages.h"

enum clip_direction {
	DIR_FORWARDS,
	DIR_BACKWARDS,
	DIR_BACK_AND_FORTH
};

struct video_clip {
	entry_ref ref;
	int32 vidtrack;
	int64 start;
	int64 stop;
	int64 last; // only valid when clip is not loaded
	bigtime_t framerate;
	clip_direction direction;
};

class VideoReader {
public:
					VideoReader();
					~VideoReader();
	
	bool			Lock() { return mutexLock.Lock(); };
	void			Unlock() { mutexLock.Unlock(); };
	
	status_t		SniffFile(entry_ref *ref);
	status_t		LoadFile(entry_ref *ref);
	// UnloadFile() needs Lock()/Unlock() around its call
	status_t		UnloadFile(int32 index);
	
	// next two functions need a Lock()/Unlock() around their call
	status_t		LoadClipForPlaying(int32 index, BBitmap **bitmap, bool seek = false);
	void			UnloadClip();
	
	// gets the video frame. needs Lock()/Unlock() around the call
	void			GetNextFrame(BBitmap *bits);
	
	void			GetClipDimensions(float *w, float *h) const;
	int32			CurrentClip() const;
	bigtime_t		ClipFramerate() const { return currentFramerate; };
	bigtime_t		NextFrameTime() const { return nextFrameTime; };
	bigtime_t		ClipLengthTime() const { return clipLengthTime; };
	int64			ClipLengthFrames() const { return clipLengthFrames; };
	int64			CurrentFrameNumber() const;
	int64			ClipStart() const;
	int64			ClipEnd() const;
	clip_direction	ClipDirection() const;
	
	// these three need Lock()/Unlock() around their call
	void			SetClipStart(int64 frame);
	void			SetClipEnd(int64 frame);
	void			SetClipDirection(clip_direction dir);
	
	void			SetUIMessenger(BView *view);
	
private:

	BList			clipList;
	BLocker			mutexLock;
	int32			currentClipIndex;
	bigtime_t		currentFramerate;
	bigtime_t		nextFrameTime;
	bigtime_t		clipLengthTime;
	int64			clipLengthFrames;
	int64			currentFrameNum;
	int64			frameDelta;
	float			width;
	float			height;
	clip_direction	playDirection;
	video_clip		*currentClip;
	
	BMessenger		*uiMessenger;
	
	BMediaFile		*clipFile;
	BMediaTrack		*videoTrack;
};

#endif