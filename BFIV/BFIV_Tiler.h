#ifndef BFIV_TILER_H
#define BFIV_TILER_H

#ifndef _OS_H
#include <OS.h>
#endif
#ifndef _FILE_H
#include <File.h>
#endif
#ifndef _ENTRY_H
#include <Entry.h>
#endif
#ifndef _TRANSLATOR_FORMATS_H
#include <TranslatorFormats.h>
#endif
#ifndef	_BITMAP_H
#include <Bitmap.h>
#endif
#ifndef _MESSENGER_H
#include <Messenger.h>
#endif
#ifndef __BSTRING__
#include <String.h>
#endif
#ifndef _BYTEORDER_H
#include <ByteOrder.h>
#endif
#ifndef _MALLOC_H
#include <malloc.h>
#endif
#ifndef _STDIO_H_
#include <stdio.h>
#endif
#ifndef MESSAGE_CONSTANTS_H
#include "MessageConstants.h"
#endif

// size of the image header
#define HEADER_SIZE sizeof(TranslatorBitmap)

// squares that are 512x512 bytes
const uint32 BFIV_CHUNK_WIDTH = 512;
const uint32 BFIV_CHUNK_HEIGHT = 512;
const uint32 BFIV_CHUNK_SIZE = (BFIV_CHUNK_WIDTH*BFIV_CHUNK_HEIGHT); // 256kb chunks


class BFIV_Tiler : public BPositionIO {
public:
									BFIV_Tiler(BMessenger *handler, const char *dir);
									~BFIV_Tiler();

/* BPositionIO hooks */
virtual	ssize_t		Read(void *buffer, size_t size);
virtual	ssize_t		ReadAt(off_t pos, void *buffer, size_t size);
virtual	ssize_t		Write(const void *buffer, size_t size);
virtual	ssize_t		WriteAt(off_t pos, const void *buffer, size_t size);

virtual off_t			Seek(off_t position, uint32 seek_mode);
virtual	off_t			Position() const { printf("Position()\n");return fPos; };

virtual status_t	SetSize(off_t size);

/* BFIV_Tiler specific stuff */
void							PartitionImage();

BRect							Bounds(float scale) const;
BRect							GetChunkBounds();
BRect							RectForChunk(uint32 index);
BRect							ScaleRect(BRect rect, uint32 index, float scale);
void							GetImageChunk(uint32 index, BBitmap *chunk);


uint32						CountChunks() const { return fNumChunks; };
void							ChunksInside(BRect rect, uint32 **list, uint32 *numchunks);

color_space				ColorSpace() const { return fColorSpace; };

void							Reset();
void							SendStatusMsg(float percentage);
void							DumpStats(BString & string);

private:
BFile							*fScratchFile;
BMessenger				*fStatusHandler;
BString						fScratchFileName;
color_space				fColorSpace;
off_t							fSize, fPos;
off_t							fLastStatus;
uint32						fHeight, fWidth;
uint32						fRowBytes, fDataSize;
uint32						fNumChunks;
uint32						fChunksHigh, fChunksWide;
uint32						fChunkHeight, fChunkWidth;
bool							fHeader;

};

// a few inlines

inline BRect BFIV_Tiler::Bounds(float scale) const
{
	if(scale > 1.0)
	{
		return BRect(0,0,
						((fWidth-1)*scale)-((scale-1.0)*fChunksWide),
						((fHeight-1)*scale)-((scale-1.0)*fChunksHigh));
	}
	else // scales < 100%
		return BRect(0,0, ((fWidth-1)*scale), ((fHeight-1)*scale));
}

inline BRect BFIV_Tiler::ScaleRect(BRect rect, uint32 index, float scale)
{
	float t=rect.top,l=rect.left,r=rect.right,b=rect.bottom;
	
	// scale all sides
	l *= scale;
	t *= scale;
	r *= scale;
	b *= scale;
	
	// adjust scaled values to avoid overlap (scale < 100%)
	// and gaps (scale > 100%)
	if(scale > 1.0)
	{
		uint32 cx, cy;
		
		// find our coordinates in "chunk-space" :)
		cx = index % fChunksWide;
		cy = index / fChunksWide;
		
		// adjust scaled values
		l -= cx*(scale-1.0);
		r -= cx*(scale-1.0);
		t -= cy*(scale-1.0);
		b -= cy*(scale-1.0);
	}
	else if(scale < 1.0)
	{
		uint32 cx, cy;
		
		// find our coordinates in "chunk-space" :)
		cx = index % fChunksWide;
		cy = index / fChunksWide;
		
		// easier than previous case
		l += float(cx);
		r += float(cx);
		t += float(cy);
		b += float(cy);
	}
	
	return BRect(l,t,r,b);
}

#endif

/*
	The Translator Beastiary:
	Or something like that......
	
	We only take an outType of B_TRANSLATOR_BITMAP sir, sorry.
	The first thing a Good Translator [tm] will write is the image header. This
	is just a TranslatorBitmap struct with data filled in. With this assumption,
	we can hack Write()/WriteAt(pos < sizeof(TranslatorBitmap), ...) to watch
	for this.
	
	Once we have a header, we can open our scratch file on disk. Better check to see
	if there's enough space first :)
	
	Break scratch file into imaginary 64kb chunks. 64kb may sound magic, but that is
	preffered chunk size for bfs last I heard. Each chunk represents a rectangle in
	image. (NOTE: make sure the algorithm that partitions the image is sufficiently
	intelligent)
	
	Write() and WriteAt() are interesting. Rows will cross multiple chunks, so
	BFile::WriteAt() is our friend. Translators that write all their data or write
	data in non-uniform way suck ass and are looked down upon by the author. ( but
	I'll still do my damnedest to deal with these Translators correctly )

*/