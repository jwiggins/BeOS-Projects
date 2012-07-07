#ifndef ABOUT_VIEW_H
#define ABOUT_VIEW_H

// interface
#include <View.h>
#include <Button.h>
#include <TextView.h>
#include <Window.h>
// mine
#include "AttrApp.h"
#include "MessageDefs.h"
#include "AppResStringDefs.h"
#include "AppResStrings.h"
#include "version.h"

class AboutView : public BView {
public:
					AboutView(BRect frame);
					~AboutView();
virtual	void		MouseDown(BPoint point);
virtual	void		Draw(BRect);
virtual	void		Pulse();

private:
BRect				magic_rect; // where to click fo da eastuh egg
char 				*version_string;
};

#endif
