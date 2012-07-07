/*
	
	SketchView.h
	
	John Wiggins 1998
	
*/

#ifndef SKETCH_VIEW_H
#define SKETCH_VIEW_H

#include <View.h>
#include <Picture.h>
#include <Bitmap.h>
#include <Path.h>
#include <File.h>
#include <Message.h>
#include <string.h>
#include "Sketch.h"
#include "drawView.h"
#include "SketchWindow.h"

class drawView;

class SketchView : public BView {

public:
				SketchView(BRect frame, char *name); 
				~SketchView();
virtual	void	AttachedToWindow();
virtual	void	Draw(BRect updateRect);
virtual	void	KeyDown(const char *bytes, int32 numbytes); // catch arrow keys
void			AnimatedErase(); // Shakes, blurs, then calls Erase()
void			Erase(); // just calls Erase()
void			Blur(); // just calls Blur()
void			SaveBitmap(BMessage *msg); // writes the bitmap to a file
uint32			find_constant(BTranslatorRoster *roster, const char *mime); // convenience

private:
BPicture		*theFrame; // the red frame
BBitmap			*the_drawing; // the buffer that holds the drawing
drawView		*monet; // a view that draws into the_drawing
						// (yup, I made a whole class just for that)
uchar			grey; // the color of the background. Used by Blur()
BRect			bitmap_rect;
};

#endif
