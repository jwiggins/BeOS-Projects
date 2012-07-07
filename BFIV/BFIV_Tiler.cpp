#include "BFIV_Tiler.h"

BFIV_Tiler::BFIV_Tiler(BMessenger *handler, const char *dir)
:BPositionIO(), fScratchFileName()
{
	fSize = fPos = fLastStatus =0;
	fNumChunks = fChunksHigh = fChunksWide = 0;
	fChunkHeight = fChunkWidth = 0;
	fHeader = false;
	fScratchFile = NULL;
	fStatusHandler = handler; // status messages go here
	
	fRowBytes = fDataSize = fHeight = fWidth = 0;
	
	// open the scratch file
	
	// first some pathname magic so we don't clash with another instance
	BString	path;
	path << dir << "/BFIV-" << system_time();
	
	// construct the file
	fScratchFile = new BFile(path.String(), B_READ_WRITE | B_FAIL_IF_EXISTS | B_CREATE_FILE );
	
	// check it
	if (fScratchFile->InitCheck() != B_OK)
	{
		// psychobilly freakout!
		debugger("Failed to create scratch file.");
	}
	
	fScratchFileName = path; // cache
}

BFIV_Tiler::~BFIV_Tiler()
{
	if (fScratchFile)
		delete fScratchFile;
		
	if (fStatusHandler)
		delete fStatusHandler;
	
	//DumpStats();
	
	// then remove from the disk
	BEntry scratchFile(fScratchFileName.String());
	scratchFile.Remove();
}

ssize_t BFIV_Tiler::Read(void *buffer, size_t size)
{
	//printf("Read()\n");
	return ReadAt(fPos, buffer, size);
}

ssize_t BFIV_Tiler::ReadAt(off_t pos, void *buffer, size_t size)
{
	//printf("ReadAt(%Ld, buffer, %d)\n", pos, size);
	size_t bytesRead = 0;
	off_t xpos, ypos, scratchpos, readRet, read_size;
	uint32 chunkx, chunky, chunknum, chunk_xoff, chunk_yoff, cur_chunk;
	char chunkrow[BFIV_CHUNK_WIDTH];
	
	pos -= HEADER_SIZE; // backup past the header that wasn't written
	
	xpos = (pos % fRowBytes);
	ypos = (pos / fRowBytes);
	
	chunkx = xpos / BFIV_CHUNK_WIDTH;
	chunky = ypos / BFIV_CHUNK_HEIGHT;
	
	chunk_xoff = xpos - (chunkx * BFIV_CHUNK_WIDTH);
	chunk_yoff = ypos - (chunky * BFIV_CHUNK_HEIGHT);
	
	chunknum = (chunky * fChunksWide) + chunkx;
	cur_chunk = chunknum;
	
	scratchpos = chunknum * BFIV_CHUNK_SIZE; // beginning of the chunk
	scratchpos += (chunk_yoff * BFIV_CHUNK_WIDTH) + chunk_xoff; // inside the chunk
	
	// get the read size
	read_size = (BFIV_CHUNK_WIDTH - chunk_xoff);
	// check for funky read sizes
	if(	((cur_chunk+1) % fChunksWide) == 0 && (fRowBytes % BFIV_CHUNK_WIDTH) != 0 )
	{
		// this is an edge chunk and it's not BFIV_CHUNK_WIDTH wide
		// cut read_size to something managable (ie: < (fRowBytes % BFIV_CHUNK_WIDTH))
		read_size = (fRowBytes % BFIV_CHUNK_WIDTH) - chunk_xoff; // will be > 0
	}
	if( read_size > size )
		read_size = size;
	
	//memset(buffer, 0, size); // zero
	memset((void *)chunkrow, 0, BFIV_CHUNK_WIDTH); // zero
	// read
	readRet =	fScratchFile->ReadAt(scratchpos, (void *)(chunkrow+chunk_xoff), read_size);
	
	// write to the buffer
	memcpy((void *)buffer, (void *)(chunkrow+chunk_xoff), read_size);
	
	// take note
	bytesRead += read_size; // used read_size instead of readRet. Could fail... (!)
	
	// get the new pos
	// do this before we enter the loop...
	
	// check to see if we hit the end of a row
	if( ((cur_chunk+1) % fChunksWide) == 0 )
	{
		// we are at the end of a row
		if((chunk_yoff+1) == fChunkHeight)
		{
			// and the end of a chunk row
			cur_chunk++; // next chunk
			chunk_yoff = 0; // top corner of the row
			scratchpos = cur_chunk * BFIV_CHUNK_SIZE; // the new scratch position
		}
		else
		{
			cur_chunk -= (fChunksWide-1); // back up to the start of the chunk row
			chunk_yoff++; // down one row
			scratchpos = cur_chunk * BFIV_CHUNK_SIZE; // the scratchpos for this chunk
			scratchpos += (chunk_yoff * BFIV_CHUNK_WIDTH); // plus the y offset
		}
	}
	else
	{
		// not at the end of a row
		cur_chunk++; // next chunk
		scratchpos = cur_chunk * BFIV_CHUNK_SIZE; // start of the chunk
		scratchpos += (chunk_yoff * BFIV_CHUNK_WIDTH); // plus the y offset
	}
			
	// read the rest
	while(readRet > B_OK && bytesRead < size)
	{			
		// how much are we reading?
		read_size = BFIV_CHUNK_WIDTH;
		// check for funky read sizes
		if(	((cur_chunk+1) % fChunksWide) == 0 && (fRowBytes % BFIV_CHUNK_WIDTH) != 0 )
		{
			// this is an edge chunk and it's not BFIV_CHUNK_WIDTH wide
			// cut read_size to something managable (ie: (fRowBytes % BFIV_CHUNK_WIDTH))
			read_size = (fRowBytes % BFIV_CHUNK_WIDTH);
		}
		// quick sanity check...
		if( read_size > (size - bytesRead) )
			read_size = (size - bytesRead);
		
		// prepare zee boofair!
		memset((void *)chunkrow, 0, BFIV_CHUNK_WIDTH); // zero		
		// read it
		readRet = fScratchFile->ReadAt(scratchpos, (void *)chunkrow, read_size);
			
		// write to the buffer
		memcpy((void *)((char *)buffer+bytesRead), (void *)chunkrow, read_size);
		
		// next pos...
		// check to see if we hit the end of a row
		if( ((cur_chunk+1) % fChunksWide) == 0 )
		{
			// we are at the end of a row
			// check for end of chunk row
			if((chunk_yoff+1) == fChunkHeight)
			{
				// we are at the end of a chunk row too
				cur_chunk++; // next chunk
				chunk_yoff = 0; // top of the row
				scratchpos = cur_chunk * BFIV_CHUNK_SIZE; // the new scratch position
			}
			else
			{
				// beginning of the chunk row, next row down
				cur_chunk -= (fChunksWide-1); // back up to the start of the chunk row
				chunk_yoff++; // down one row
				// now find the scratch pos
				scratchpos = cur_chunk * BFIV_CHUNK_SIZE; // the scratchpos for this chunk
				scratchpos += (chunk_yoff * BFIV_CHUNK_WIDTH); // plus the y offset
			}
		}
		else
		{
			// not at the end of a row
			cur_chunk++; // next chunk
			scratchpos = cur_chunk * BFIV_CHUNK_SIZE; // start of the chunk
			scratchpos += (chunk_yoff * BFIV_CHUNK_WIDTH); // plus the y offset
		}
		
		// take note
		bytesRead += read_size;
	}
	
	fPos += bytesRead;
	
	return bytesRead;
}



ssize_t BFIV_Tiler::Write(const void *buffer, size_t size)
{
	//printf("Write()\n");
	return WriteAt(fPos, buffer, size);
}

ssize_t BFIV_Tiler::WriteAt(off_t pos, const void *buffer, size_t size)
{
	//printf("WriteAt(%Ld, buffer, %d)\n", pos, size);
	size_t bytesWritten = 0;
	
	if ( !fHeader && pos < HEADER_SIZE ) // the header
	{
		//printf("WriteAt(). Stop! Header time....\n");
		if (pos != 0) // sanity check
		{
			debugger("Bad Write(). Pos < sizeof(Header) and Pos != 0.");
			// we don't get here. Least I hope we don't :) 
		}
		else
		{
			// allocate a header
			TranslatorBitmap header;
			
			// copy the data
			memcpy((void *)&header, buffer, HEADER_SIZE);
			size -= HEADER_SIZE;
			
			//printf("header.magic = %x : B_TRANSLATOR_BITMAP = %x\n",
			//header.magic, B_HOST_TO_BENDIAN_INT32(B_TRANSLATOR_BITMAP));
			
			//header.bounds.PrintToStream();
			
			// swapping the rect
			header.bounds.left = B_BENDIAN_TO_HOST_FLOAT(header.bounds.left);
			header.bounds.top = B_BENDIAN_TO_HOST_FLOAT(header.bounds.top);
			header.bounds.right = B_BENDIAN_TO_HOST_FLOAT(header.bounds.right);
			header.bounds.bottom = B_BENDIAN_TO_HOST_FLOAT(header.bounds.bottom);
			
			//header.bounds.PrintToStream();
			
			// keep all the info we need
			fRowBytes = header.rowBytes;
			fDataSize = header.dataSize;
			fColorSpace = header.colors;
			fHeight = header.bounds.IntegerHeight();
			fWidth = header.bounds.IntegerWidth();
			
			// Header is big endian!!!
			// swap your fucking data!
			fRowBytes = B_BENDIAN_TO_HOST_INT32(fRowBytes);
			fDataSize = B_BENDIAN_TO_HOST_INT32(fDataSize);
			fColorSpace = (color_space)B_BENDIAN_TO_HOST_INT32(fColorSpace);
			
//			printf("fRowBytes = %d, fDataSize = %d, fHeight = %d, fWidth = %d\n",
//			 fRowBytes,fDataSize,fHeight,fWidth);

			// calculate the bonehead quotient for this translator
//			if (size == 0)
//			{
//				// all done. yay.
//				printf("Low bonehead quotient\n");
//			}
//			else
//			{
//				// what kinda weird sonofabitch writes more than a header's worth when writing
//				// a header?
//				printf("High bonehead quotient\n");
//			}
			
			// ok. We know what the image's dimensions are.
			// Partition the image into intelligently shaped chunks
			PartitionImage();
			
			fHeader = true; // seen the header
			bytesWritten += HEADER_SIZE;
			fPos += HEADER_SIZE;
		}
	}
	else // normal image data (everything after the header)
	{
		//printf("WriteAt() Image data.\n");
		// xpos = (pos % rowbytes)
		// ypos = (pos / rowbytes)
		// chunkx = xpos / BFIV_CHUNK_WIDTH
		// chunky = ypos / BFIV_CHUNK_HEIGHT
		// chunknum = (chunky * fChunksWide) + chunkx
		off_t xpos, ypos, scratchpos, writeRet, write_size;
		uint32 chunkx, chunky, chunknum, chunk_xoff, chunk_yoff, cur_chunk;
		char chunkrow[BFIV_CHUNK_WIDTH];
		
		pos -= HEADER_SIZE; // backup.. we didn't write the header to disk
		
		xpos = (pos % fRowBytes);
		ypos = (pos / fRowBytes);
		
		chunkx = xpos / BFIV_CHUNK_WIDTH;
		chunky = ypos / BFIV_CHUNK_HEIGHT;
		
		chunk_xoff = xpos - (chunkx * BFIV_CHUNK_WIDTH);
		chunk_yoff = ypos - (chunky * BFIV_CHUNK_HEIGHT);
		
		chunknum = (chunky * fChunksWide) + chunkx;
		cur_chunk = chunknum;
		
		scratchpos = chunknum * BFIV_CHUNK_SIZE; // beginning of the chunk
		scratchpos += (chunk_yoff * BFIV_CHUNK_WIDTH) + chunk_xoff; // inside the chunk
		
		// basic write size
		write_size = (BFIV_CHUNK_WIDTH - chunk_xoff);
		// funky write sizes
		if(	((cur_chunk+1) % fChunksWide) == 0 && (fRowBytes % BFIV_CHUNK_WIDTH) != 0 )
		{
			// ok, this is an edge chunk and it's not BFIV_CHUNK_WIDTH
			// cut write_size to something managable
			write_size = (fRowBytes % BFIV_CHUNK_WIDTH) - chunk_xoff; // will be > 0
		}
		if( write_size > size )
			write_size = size;
		
		memset((void *)chunkrow, 0, BFIV_CHUNK_WIDTH); // zero
		memcpy((void *)(chunkrow + chunk_xoff), buffer, write_size); // copy
		
		//printf("WriteAt(%Ld, chunkrow, %Ld)\n", scratchpos, write_size);
		// write
		writeRet =
			fScratchFile->WriteAt(scratchpos, (void *)(chunkrow + chunk_xoff), write_size);
		
		// take note
		bytesWritten += write_size; // used write_size instead of writeRet. Could fail... (!)
		
		// get the new pos
		// do this before we enter the loop...
		
		// check to see if we hit the end of a row
		if( ((cur_chunk+1) % fChunksWide) == 0 )
		{
			// we are at the end of a row
			if((chunk_yoff+1) == fChunkHeight)
			{
				// and the end of a chunk row
				cur_chunk++; // next chunk
				chunk_yoff = 0; // top corner of the row
				scratchpos = cur_chunk * BFIV_CHUNK_SIZE; // the new scratch position
			}
			else
			{
				cur_chunk -= (fChunksWide-1); // back up to the start of the chunk row
				chunk_yoff++; // down one row
				scratchpos = cur_chunk * BFIV_CHUNK_SIZE; // the scratchpos for this chunk
				scratchpos += (chunk_yoff * BFIV_CHUNK_WIDTH); // plus the y offset
			}
		}
		else
		{
			// not at the end of a row
			cur_chunk++; // next chunk
			scratchpos = cur_chunk * BFIV_CHUNK_SIZE; // start of the chunk
			scratchpos += (chunk_yoff * BFIV_CHUNK_WIDTH); // plus the y offset
		}
				
		// write the rest
		while(writeRet > B_OK && bytesWritten < size)
		{			
			// how much are we writing?
			// basic write size
			write_size = BFIV_CHUNK_WIDTH;
			// funky write sizes
			if(	(((cur_chunk+1) % fChunksWide) == 0) && ((fRowBytes % BFIV_CHUNK_WIDTH) != 0) )
			{
				// ok, this is an edge chunk and it's not BFIV_CHUNK_WIDTH
				// cut write_size to something managable
				write_size = (fRowBytes % BFIV_CHUNK_WIDTH); // will be > 0
			}
			if( write_size > (size - bytesWritten) )
				write_size = (size - bytesWritten);
			
			// prepare zee boofair!
			memset((void *)chunkrow, 0, BFIV_CHUNK_WIDTH); // zero
			memcpy((void *)chunkrow, (void *)((char *)buffer + bytesWritten), write_size); // copy
			
			//printf("WriteAt(%Ld, chunkrow, %Ld)\n", scratchpos, write_size);
			// write it
			writeRet = fScratchFile->WriteAt(scratchpos, (void *)chunkrow, write_size);
			
			// next pos
			
			// check to see if we hit the end of a row
			if( ((cur_chunk+1) % fChunksWide) == 0 )
			{
				// we are at the end of a row
				// check for end of chunk row
				if((chunk_yoff+1) == fChunkHeight)
				{
					// we are at the end of a chunk row too
					cur_chunk++; // next chunk
					chunk_yoff = 0; // top of the row
					scratchpos = cur_chunk * BFIV_CHUNK_SIZE; // the new scratch position
				}
				else
				{
					// beginning of the chunk row, next row down
					cur_chunk -= (fChunksWide-1); // back up to the start of the chunk row
					chunk_yoff++; // down one row
					// now find the scratch pos
					scratchpos = cur_chunk * BFIV_CHUNK_SIZE; // the scratchpos for this chunk
					scratchpos += (chunk_yoff * BFIV_CHUNK_WIDTH); // plus the y offset
				}
			}
			else
			{
				// not at the end of a row
				cur_chunk++; // next chunk
				scratchpos = cur_chunk * BFIV_CHUNK_SIZE; // start of the chunk
				scratchpos += (chunk_yoff * BFIV_CHUNK_WIDTH); // plus the y offset
			}
			
			// take note
			bytesWritten += write_size;
		}
		
		
		// and before we return...
		fPos += bytesWritten;
	
		if ((fPos - fLastStatus) >= off_t(1.0 / (float)fDataSize))
		{
			// make the status bar move
			SendStatusMsg( (fPos - fLastStatus) / (float)fDataSize);
			fLastStatus = fPos;
		}
	}
	
	return bytesWritten;
}

off_t BFIV_Tiler::Seek(off_t position, uint32 seek_mode)
{
	//printf("Seek()\n");
	switch(seek_mode)
	{
		case SEEK_SET:
		{
			//printf("SEEK_SET: position = %Ld\n", position);
			fPos = position;
			break;
		}
		case SEEK_CUR:
		{
			//printf("SEEK_CUR: position += %Ld\n", position);
			fPos += position;
			break;
		}
		case SEEK_END:
		{
			//printf("SEEK_END: position = [end] + %Ld\n", position);
			fPos = fSize + position;
			break;
		}
		default:
		{
			// What to do?
			
			// Do nothing! The bastards should know better... Make their debugging time
			// hell... Muhahahahah
			break;
		}
	}
	
	return fPos;
}

status_t BFIV_Tiler::SetSize(off_t size)
{
	//printf("SetSize()\n");
	fSize = size;
	return B_OK;
}

void BFIV_Tiler::PartitionImage()
{
	//printf("PartitionImage() 1\n");
	// ooh fun. This is half the brains of the entire class
	// don't fuck up.
	// the whole world is watching
	// quit writting comments and get on with the code already
	fChunkWidth = BFIV_CHUNK_WIDTH / (fRowBytes/fWidth);
	fChunkHeight = BFIV_CHUNK_HEIGHT;
	
	//printf("PartitionImage() 2\n");
	// need to know:
	// chunk count
	// chunks per row, chunks per column
	// height and width of chunks (in pixels)
	//
	// some chunks at the right and bottom edges may 'hang off' the
	// edge as it were. Meaning that there just wouldn't be any data there
	
	fChunksWide = fWidth / fChunkWidth; // how many chunks across?
	fChunksWide += (fWidth % fChunkWidth) ? 1 : 0; // round up for extra pixels
	fChunksHigh = fHeight / fChunkHeight; // and how many chunks tall?
	fChunksHigh += (fHeight % fChunkHeight) ? 1 : 0; // round up for extra pixels
	
	fNumChunks = fChunksWide * fChunksHigh;
	
	if(fScratchFile->SetSize(fNumChunks * BFIV_CHUNK_SIZE) != B_OK)
		debugger("Not enough room on disk for scratch file.");
}

BRect BFIV_Tiler::GetChunkBounds()
{
	return BRect(0,0, (fChunkWidth-1), (fChunkHeight-1));
}

BRect BFIV_Tiler::RectForChunk(uint32 index)
{
	uint32 left, top;
	uint32 height, width;
	
	// find our coordinates in "chunk-space" :)
	left = index % fChunksWide;
	top = index / fChunksWide;
	
	// find out if we're on an edge
	if ((left + 1) == fChunksWide)
	{
		//printf("chunk %d is on an edge.\n", index);
		// yes, we are on an edge
		// do we fill the entire rect?
		if((fWidth % fChunkWidth) != 0)
		{
			//printf("width is less than fChunkWidth\n");
			//printf("fWidth %% fChunkWidth = %d\n", fWidth % fChunkWidth);
			// no! what a shocker...
			width = (fWidth % fChunkWidth);
		}
		else
			width = fChunkWidth;
	}
	else
		width = fChunkWidth;
	
	if ((top + 1) == fChunksHigh)
	{
		//printf("chunk %d is on an edge.\n", index);
		// check the height
		if ((fHeight % fChunkHeight) != 0)
		{
			//printf("height is less than fChunkHeight\n");
			//printf("fHeight %% fChunkHeight = %d\n", fHeight % fChunkHeight);
			// the height ain't right! :P
			height = (fHeight % fChunkHeight);
		}
		else
			height = fChunkHeight;
	}
	else
		height = fChunkHeight;
	
	// get the real values for the top corner
	left *= fChunkWidth;
	top *= fChunkHeight;
	
	return BRect(left, top,	(left + (width-1)), (top + (height-1)));
}

void BFIV_Tiler::GetImageChunk(uint32 index, BBitmap *chunk)
{
	off_t offset = (index * BFIV_CHUNK_SIZE); // where from?
	
	fScratchFile->ReadAt(offset, chunk->Bits(), BFIV_CHUNK_SIZE); // fill 'er up
}

void BFIV_Tiler::ChunksInside(BRect rect, uint32 **list, uint32 *numchunks)
{
	////////////
	// UNTESTED
	////////////
	
	// find all the chunks that 'rect' touches
	// return a list
	
	// steps:
	// get a chunk row # for rect.top and rect.bottom
	// get a chunk column # for rect.left and rect.right
	// get all chunks in the sub-grid made by the
	// found chunk rows and chunk columns
	uint32 ctop, cbottom, cleft, cright;
	uint32 gheight, gwidth;
	
	// get the bounds for the grid
	ctop = uint32(rect.top) / fChunkHeight;
	cbottom = uint32(rect.bottom) / fChunkHeight;
	cleft = uint32(rect.left) / fChunkWidth;
	cright = uint32(rect.right) / fChunkWidth;

	// the height in chunks
	gheight = (cbottom - ctop);
	gheight = gheight ? gheight : 1; // at least one row high
	// the width in chunks
	gwidth = (cright - cleft);
	gwidth = gwidth ? gwidth : 1; // at least one column wide
	
	*numchunks = gwidth * gheight; // fill in the chunk count
	*list = (uint32 *)malloc( *numchunks * sizeof(uint32) ); // allocate the space
	
	for(uint32 i=cleft; i < cright; i++)
	{
		for(uint32 j=ctop; j < cbottom; j++)
		{
			// add it to the list
			uint32 *addr = (*list + (((i-cleft)*gwidth) + (j-ctop)));
			*addr = (ctop*fChunksWide) + cleft;
		}
	}
}

void BFIV_Tiler::Reset()
{
	// reset the object
	// state is the same as it was after construction
	fSize = fPos = fLastStatus = 0;
	fRowBytes = fDataSize = fHeight = fWidth = 0;
	fNumChunks = fChunksHigh = fChunksWide = 0;
	fHeader = false;
	
	fScratchFile->SetSize(fSize); // 0 bytes
}

void BFIV_Tiler::SendStatusMsg(float percentage)
{
	BMessage msg(IMAGE_STATUS_UPDATE);
	
	msg.AddFloat("delta", percentage); // add the delta
	if(fStatusHandler->IsValid()) // sanity check
		fStatusHandler->SendMessage(&msg); // deliver
}

void BFIV_Tiler::DumpStats(BString & string)
{
	string << "Dimensions: " << fHeight << " x " << fWidth << '\n';
	string << "Scratch File: " << fScratchFileName.String() << '\n';
	string << "Scratch Size: "
				 << (fNumChunks * BFIV_CHUNK_SIZE) / float(1024*1024) << " MB\n";
	string << "Data Size: " << fDataSize / float(1024*1024) << " MB\n";
}