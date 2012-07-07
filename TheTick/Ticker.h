/*
	Ticker.h - generic LED-ish ticker display
	
	TheTick by John Wiggins. 1999

*/

#ifndef TICKER_H
#define TICKER_H

#include <View.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "MessageDefs.h"
#include "TickerView.h"

#define LETTERWIDTH 5
#define LETTERHEIGHT 6
#define LETTERSPERROW 20
#define ROWLENGTH (LETTERSPERROW*LETTERWIDTH)

class TickerView;

class _EXPORT Ticker : public BView {
public:
_EXPORT 						Ticker(BRect frame, TickerView *tv);
_EXPORT 						~Ticker();
_EXPORT virtual void				AttachedToWindow();
_EXPORT int						quasi_pulse(int, int);
_EXPORT void						clear();

private:
_EXPORT inline void				draw_dot(int , int , int , int);
char						current_char;
TickerView				*parent;
};

inline void Ticker::draw_dot(int maj_column, int min_column, int row, int value)
{

	// maj_column starts at 0 and goes to 19
	// min_column starts at 0 and goes to 4
	// row starts at 0 and goes to 5
	// value is either a 1 or a 0
	
	int place;
	BRect rect(0,0,3,3);
	place = ((row * ROWLENGTH)+(maj_column * LETTERWIDTH)+min_column);
	if((place >= 0) && (place <= (ROWLENGTH*LETTERHEIGHT)))
	{
		// draw it!
		rect.OffsetTo(((maj_column*LETTERWIDTH*5.0)+(min_column*5.0)), (row*5.0)+1.0);
		if(value)
			FillEllipse(rect);
		else
			FillEllipse(rect, B_SOLID_LOW);
	}
}

#endif