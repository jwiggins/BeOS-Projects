/*
	WhackerFilt.h
	WhackerFilt class for ChannelWhacker filter plugin
	2000 John Wiggins
*/

#ifndef WHACKER_FILT_H
#define WHACKER_FILT_H

#include <interface/Bitmap.h>

class WhackerFilt {
public:
					WhackerFilt();
	
	void			Filter(BBitmap *frame);
//	void			ToggleRed(bool state) { redChan = state; };
//	void			ToggleGreen(bool state) { greenChan = state; };
//	void			ToggleBlue(bool state) { blueChan = state; };
	void			SetRed(float val) { redChan = val; }
	void			SetGreen(float val) { greenChan = val; }
	void			SetBlue(float val) { blueChan = val; }
	
private:
	
//	bool			redChan, greenChan, blueChan;
	float			redChan, greenChan, blueChan;
};

#endif