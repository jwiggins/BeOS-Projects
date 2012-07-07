/*
	Ticker.cpp
	
	TheTick by John Wiggins. 1999

*/

#include "Ticker.h"
#include "Glyph.h"

Ticker::Ticker(BRect frame, TickerView *tv)
: BView(frame, "Ticker", B_FOLLOW_ALL, 0)
{
	parent = tv;
	current_char = ' ';
}

Ticker::~Ticker()
{
	//printf("Ticker dtor\n");
}

void Ticker::AttachedToWindow()
{
	FillRect(Bounds(), B_SOLID_HIGH); // high color starts out as black
	Flush();
	SetLowColor(64,64,64,255); // dark grey - off
	SetHighColor(192,0,0,255); // darkish red - on
	clear();
}

// change lowercase to caps and ignore any ascii > 'Z'
#define transform(c) ((((c >= 65) && (c <= 90))) ? c : \
					(((c > 96) && (c < 123)) ? c - 32 : \
					(((c >= 32) && (c <= 64)) ? c : ' ')))

int Ticker::quasi_pulse(int column, int speed)
{
	//printf("quasi_pulse(%d)\ncurrent_char = %c\n", column, current_char);
	
	// column is the vertical column in the current glyph that we are drawing
	// speed is the number of columns to be drawn
	// we return column when we're done
	
	// Scroll the display 5*speed pixels to the left
	BRect src(Bounds()), dst;
	src.top += 1.0; // top line doesn't need to be scrolled
	dst = src;
	src.left += 5.0*speed;
	dst.right -= 5.0*speed;
	CopyBits(src, dst);
	
	// Draw 'speed' columns of dots on the far right of the ticker
	for(int j=4-(speed-1);j<5;j++) // loop for the number of columns that need to be drawn
	{
	 for(int k=0;k<6;k++) // row
	  draw_dot(19,j,k,(glyphlist[transform(current_char)-32].bits & ((0x80000000)>>((k*5)+column)) ));
	 
	 if(column == 4) // end of the current char. grab a fresh one.
	 {
	  column = 0; // reset column
	  current_char = parent->next_char(); // parent is the BView that owns the bitmap that we draw into
	 }
	 else if(j < 5) // if we still have columns left to draw, iterate column
	  column++;
	}
	Sync();
	
	return column;
}

// turn all the dots "off" in the ticker
void Ticker::clear()
{
	for(int i=0;i<20;i++) // maj_column
	 for(int j=0;j<5;j++) // min_column
	  for(int k=0;k<6;k++) // row
	   draw_dot(i,j,k,0); // Clears the dot
	Flush();
	
	current_char = ' '; // so we don't draw half of a zombie glyph
}