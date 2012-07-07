/*
	TickerWindow.h
	
	TheTick by John Wiggins. 1999

*/

#ifndef TICKER_WINDOW_H
#define TICKER_WINDOW_H

// app kit
#include <Application.h>
// interface kit
#include <Window.h>
// storage kit
#include <Node.h>
#include <Path.h>
#include <Directory.h>
#include <FindDirectory.h>
// posix
#include <string.h>
// mine
#include "TickerView.h"

class TickerWindow : public BWindow {
public:
				TickerWindow();
virtual bool		QuitRequested();
};

#endif