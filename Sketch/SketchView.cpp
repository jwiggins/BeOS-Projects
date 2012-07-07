/*
	
	SketchView.cpp
	
	John Wiggins 1998
	
*/

#include "SketchView.h"


SketchView::SketchView(BRect rect, char *name)
	   	   : BView(rect, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
}

SketchView::~SketchView()
{
	// clean up
	delete theFrame; // the picture
	delete the_drawing; // the bitmap
}

void SketchView::AttachedToWindow()
{
	// make a copy of Bounds() for code prettyness (and possibly size since Bounds() is inline)
	BRect		rect, bounds =  Bounds();
	bitmap_rect.Set(31,31, bounds.Width() - 31,bounds.Height() - 31);
	SetFont(be_bold_font);
	SetFontSize(22);
	SetViewColor(220,220,220);
	
	// set up the frame picture
	BeginPicture(new BPicture);
	// the red border
	SetHighColor(255,0,0);
	rect.Set(0,0,30,bounds.Height());
	FillRect(rect);
	rect.Set(0,0,bounds.Width(),30);
	FillRect(rect);
	rect.Set(bounds.Width() - 30,0,bounds.Width(),bounds.Height());
	FillRect(rect);
	rect.Set(0,bounds.Height() - 30,bounds.Width(),bounds.Height());
	FillRect(rect);
	// the diagonal lines in the corners
	SetHighColor(0,0,0);
	BeginLineArray(4);
	AddLine(BPoint(0,0), BPoint(30,30),HighColor());
	AddLine(BPoint(bounds.Width(),0), BPoint(bounds.Width() - 30,30),HighColor());
	AddLine(BPoint(0,bounds.Height()), BPoint(30,bounds.Height() - 30),HighColor());
	AddLine(BPoint(bounds.Width(),bounds.Height()), BPoint(bounds.Width() - 30,bounds.Height() - 30),HighColor());
	EndLineArray();
	// the inner box
	rect.Set(30,30, bounds.Width() - 30,bounds.Height() - 30);
	StrokeRect(rect);
	// the two nobs
	SetHighColor(255,255,255);
	rect.Set(35,bounds.Height() - 26, 55, bounds.Height() - 6);
	FillEllipse(rect);
	rect.Set(bounds.Width() - 55,bounds.Height() - 26, bounds.Width() - 35, bounds.Height() - 6);
	FillEllipse(rect);
	// the label
	SetHighColor(0,0,0);
	SetLowColor(255,0,0); // low color to red for correct anti-aliasing
	// the string width stuff centers the label
	DrawString("Etch-A-Sketch" B_UTF8_REGISTERED, BPoint((bounds.Width()/2) - (StringWidth("Etch-A-Sketch" B_UTF8_REGISTERED)/2), bounds.Height() - 5));
	theFrame = EndPicture();
	// end picture
	
	// i don't like this next part, but then I also don't know how 
	// to get indexed colors from an rgb_color struct....
	
	// compute the value of the grey we use for the drawing's background
	// Blur() will need this value for its work
	BBitmap		*bitmap = new BBitmap(BRect(0,0,1,1), B_COLOR_8_BIT, true);
	drawView	*filler = new drawView(BRect(0,0,1,1));
	bitmap->AddChild(filler);
	// adding filler as a child made the bitmap the right color.
	uchar		*bits = (uchar *)bitmap->Bits();
	grey = bits[0];
	delete bitmap; // also deletes filler
		
	// construct the bitmap.... 
	rect.Set(0,0, bounds.Width() - 62,bounds.Height() - 62);
	the_drawing = new BBitmap(rect, B_COLOR_8_BIT, true);
	monet = new drawView(rect);
	the_drawing->AddChild(monet);
	MakeFocus(true); // make ourselves the focus so that we catch keydowns
	/////////////////////////////////////////////////////////////////
	
	SetHighColor(0,0,0);
	SetLowColor(255,255,255);
}


void SketchView::Draw(BRect updateRect)
{
	// read the BeBook if you want to understand this test section
	// basically, if updateRect is entirely inside bitmap_rect, then
	// the result of the &(intersection) operation will equal updateRect.
	// this is here to test for Invalidate rects that im using in KeyDown()
	BRect test_rect = updateRect & bitmap_rect;
	if(test_rect == updateRect) // the updateRect is entirely within the bitmap
	{
		updateRect.OffsetBy(-(bitmap_rect.left),-(bitmap_rect.top));
		DrawBitmap(the_drawing, updateRect, test_rect);
	}
	else
	{
		BRect	bitmap_region(updateRect & bitmap_rect);
		bitmap_region.OffsetBy(-(bitmap_rect.left),-(bitmap_rect.top));
								 // convert to the bitmap's coordinate system
		DrawBitmapAsync(the_drawing, bitmap_region, bitmap_rect & updateRect);
		MovePenTo(0,0); // go here or suffer the consequences
		DrawPicture(theFrame);
		Sync();
	}
}

void SketchView::KeyDown(const char *bytes, int32 numbytes)
{
	// if you can make it do nice curves, send me mail at john@ubermensch.net
	// lines
	if(numbytes == 1)
	{
	
		int32 pen_x = (int32)((drawView *)monet)->PenLocation().x, 
				pen_y = (int32)((drawView *)monet)->PenLocation().y;
		switch(bytes[0])
		{
			// check to make sure you arent out of bounds everytime before drawing
				
			// all invalidate calls in this switch() pass a rect that is the intersection
			// of the bitmap_rect and a 6x6(7x7?) rect enclosing the PenLocation()
			case B_UP_ARROW:
				if(pen_y > 0)
				{
					if(the_drawing->Lock()) // lock it or pay the price
					{
						monet->up();
						the_drawing->Unlock();
						// grab the new pen coordinates
						pen_x = (int32)((drawView *)monet)->PenLocation().x; 
						pen_y = (int32)((drawView *)monet)->PenLocation().y;
						// draw so the changes can be seen
						Invalidate(BRect(bitmap_rect.left + pen_x - 3,
									bitmap_rect.top + pen_y - 3,
									bitmap_rect.left + pen_x + 3,
									bitmap_rect.top + pen_y + 3) & bitmap_rect);
					}
				}
				break;
			case B_DOWN_ARROW:
				if(pen_y < (bitmap_rect.Height()))
				{
					if(the_drawing->Lock()) // lock it or pay the price
					{
						monet->down();
						the_drawing->Unlock();
						// grab the new pen coordinates
						pen_x = (int32)((drawView *)monet)->PenLocation().x; 
						pen_y = (int32)((drawView *)monet)->PenLocation().y;
						// draw so the changes can be seen
						Invalidate(BRect(bitmap_rect.left + pen_x - 3,
									bitmap_rect.top + pen_y - 3,
									bitmap_rect.left + pen_x + 3,
									bitmap_rect.top + pen_y + 3) & bitmap_rect);
					}
				}
				break;
			case B_RIGHT_ARROW:
				if(pen_x < (bitmap_rect.Width()))
				{
					if(the_drawing->Lock()) // lock it or pay the price
					{
						monet->right();
						the_drawing->Unlock();
						// grab the new pen coordinates
						pen_x = (int32)((drawView *)monet)->PenLocation().x; 
						pen_y = (int32)((drawView *)monet)->PenLocation().y;
						// draw so the changes can be seen
						Invalidate(BRect(bitmap_rect.left + pen_x - 3,
									bitmap_rect.top + pen_y - 3,
									bitmap_rect.left + pen_x + 3,
									bitmap_rect.top + pen_y + 3) & bitmap_rect);
					}
				}
				break;
			case B_LEFT_ARROW:
				if(pen_x > 0)
				{
					if(the_drawing->Lock()) // lock it or pay the price
					{
						monet->left();
						the_drawing->Unlock();
						// grab the new pen coordinates
						pen_x = (int32)((drawView *)monet)->PenLocation().x; 
						pen_y = (int32)((drawView *)monet)->PenLocation().y;
						// draw so the changes can be seen
						Invalidate(BRect(bitmap_rect.left + pen_x - 3,
									bitmap_rect.top + pen_y - 3,
									bitmap_rect.left + pen_x + 3,
									bitmap_rect.top + pen_y + 3) & bitmap_rect);
					}
				}
				break;
			default:
				BView::KeyDown(bytes,numbytes);
				break;
		}
	}
}

void SketchView::AnimatedErase()
{
	// move, blur, move, blur, move, blur.... you get the idea
	Window()->MoveBy(0, 50);
	Blur();
	snooze(80 * 1000);
	Window()->MoveBy(0, -50);
	Blur();
	snooze(80 * 1000);
	Window()->MoveBy(0, 50);
	Blur();
	snooze(80 * 1000);
	Window()->MoveBy(0, -50);
	Blur();
	snooze(80 * 1000);
	Erase(); // finish it off
}

void SketchView::Erase()
{
	if(the_drawing->Lock())
	{
		monet->draw(); // draw == erase too
		the_drawing->Unlock();
		// redraw so changes show up
		Invalidate(bitmap_rect);
	}
}

void SketchView::Blur()
{
	// ok, this blur function is reeeeaaaallly simple
	// so simple in fact that its results will change if the 8bit clut changes
	
	// basically, in the 8bit palette, 0 - 31 are greyscale values
	// we stick within these values and everything is peachy
	int32	bitlength = the_drawing->BitsLength();
	int32	counter = 0; 
	uchar	*bits = (uchar *)the_drawing->Bits(); // bitmap buffer
	if(the_drawing->Lock()) // for the love of god! lock that window (er, Bitmap :)!
	{
		// do the blur 
		while(counter <= bitlength)
		{
			// colors go from black->white, so darker colors 
			// have a smaller value than our background
			if(bits[counter] < grey)
				bits[counter++] += 5; // i found that 5 looks best
			else
				bits[counter++] = grey;
		}
		// we're done. unlock the bitmap
		the_drawing->Unlock();
		// redraw the rect containing the bitmap
		Invalidate(bitmap_rect);
	}
}

void SketchView::SaveBitmap(BMessage *msg) 
{
	// This is _always_ called from MessageRecieved(). Therefore the window is _already_ locked.
	// big nasty save function
	//printf("begin SaveBitmap()\n");
	entry_ref			directory;
	char				*name; // the name of the file
	BPath				filename;
	status_t 			err = B_NO_ERROR;
    BTranslatorRoster	*roster = BTranslatorRoster::Default();
	//printf("::SaveBitmap(), set up some variables\n");
	msg->FindRef("directory", &directory); // no error checking yet
	//printf("ork\n");
	msg->FindString("name", (const char **)&name);
	//printf("::SaveBitmap(), got the entry_ref and filename\n");
	BEntry				entry(&directory);
	entry.GetPath(&filename);
	//printf("::SaveBitmap(), made a path out of the entry.\n");
	filename.Append(name);
	//printf("::SaveBitmap(), tacked the filename onto the path\n");
	//printf("file: %s \n", filename.Path());
	
	// most of this is straight out of the BeBook
	
	// add some error checking down here
	if(the_drawing->Lock())
	{
		//printf("format: %s \n", ((SketchWindow *)Window())->format);
		BBitmapStream 		stream(the_drawing); // init with contents of bitmap
   		BFile 				file(filename.Path(), B_CREATE_FILE | B_WRITE_ONLY); 
    	uint32 				type = find_constant(roster, ((SketchWindow *)Window())->format); // this will change
   		
   		//printf("type: %d\n", type); // print and stuff
   		
   		if(file.InitCheck() < B_NO_ERROR)
   			return; // bad file, get out.
    	err = roster->Translate(&stream, NULL, NULL, &file, type);
    	stream.DetachBitmap(&the_drawing);
    	the_drawing->Unlock();
    	// after the fact, check for any bad mojo
    	//printf("Translate(): %s.\n", strerror(err));
    }
}

// completely ganked out of the BeBook, modified slighty
uint32 SketchView::find_constant(BTranslatorRoster *roster, const char *mime) 
{ 
      translator_id *translators; 
      int32 num_translators; 
      
      roster->GetAllTranslators(&translators, &num_translators); 
   
      for (int32 i=0;i<num_translators;i++) 
      { 
         const translation_format *fmts; 
         int32 num_fmts; 
   
         roster->GetOutputFormats(translators[i], &fmts, &num_fmts); 
   
         for (int32 j=0;j<num_fmts;j++) 
         { 
            if (!strcasecmp(fmts[j].MIME, mime)) 
            	return fmts[j].type; 
		 } 
	  } 
	  return 0;
}