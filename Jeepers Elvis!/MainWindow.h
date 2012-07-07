/*
	MainWindow.h
	MainWindow class declaration for Jeepers Elvis!
	2000 John Wiggins
*/

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <interface/Window.h>
#include <interface/View.h>
#include <interface/Menu.h>
#include <interface/MenuBar.h>
#include <interface/MenuItem.h>
#include <interface/Button.h>
#include <interface/PictureButton.h>
#include <interface/Box.h>
#include <interface/ScrollView.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <storage/Resources.h>
#include <posix/stdio.h>

#include "MessageConstants.h"
#include "FilterListView.h"
#include "DropView.h"
#include "PluginUIWindow.h"
#include "Bitmaps.h"

class MainWindow : public BWindow {
public:
						MainWindow(BRect frame, const char *name);
	
	virtual	void		MessageReceived(BMessage *msg);
	virtual	bool		QuitRequested();

	void				PluginDropped(BMessage *msg);
	void				UnloadPlugin(int32 type, int32 which);
	void				UnloadPluginUI(int32 type, int32 which, BMessage &msg);
	void				MoveFilterPlugin(int32 from, int32 to);
private:
	void				MakePluginUIWindows();
	void				Register();
	void				Unregister();
	
	class MainView : public BView {
	public:
						MainView(BRect frame, float menuBottom);
						//~MainView();
		
		virtual	void	AttachedToWindow();
		virtual	void	Draw(BRect update);
		
		void			PluginLoaded(const char *label, int32 type, int32 which);
	private:
	
		BRect			source1Rect, source2Rect;
		BRect			mixerRect, outputRect;
		BRect			filtersRect;
	public:
		DropView		*source1, *source2;
		DropView		*mixer;
		DropView		*output;
		FilterListView	*filters;
		BPictureButton	*src1Start, *src1Stop, *src2Start, *src2Stop;
	};
	
	MainView			*mainView;
	PluginUIWindow		*src1Win, *src2Win, *mixWin, *filtWin, *outWin;
	bool				okToClose;
};

#endif