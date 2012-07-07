/*
	main.cpp - get the ball rolling
	
	TheTick by John Wiggins. 1999

*/

#include <Application.h>

#include "TickerWindow.h"

int main(void)
{
	BApplication app(APP_SIGNATURE);
	
	new TickerWindow();
	app.Run();
	
	return B_OK;
}