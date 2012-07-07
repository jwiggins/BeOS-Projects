/*
	TickerWindow.cpp
	
	TheTick by John Wiggins. 1999

*/

#include "TickerWindow.h"

TickerWindow::TickerWindow()
: BWindow(BRect(0,0,499,29), "News Ticker", B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_ZOOMABLE|B_NOT_RESIZABLE)
{
	TickerView *view = new TickerView(Bounds());
	AddChild(view);
	
	// read the top-left corner of the frame rectangle from 
	// the App's settings file
	BNode node;
	BPath path;
	BPoint point;
	ssize_t bytes_read=0;
	
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("x-vnd.Prok-TheTick");
	
	// check to see if our settings directory exists
	if(BDirectory(path.Path()).InitCheck() == B_ENTRY_NOT_FOUND)
		create_directory(path.Path(), 0777); // create it; full perms
	
	path.Append("AppSettings");
	
	node.SetTo(path.Path());
	printf("path = %s\n", path.Path());
	if(node.InitCheck() == B_OK)
		if((bytes_read = node.ReadAttr("topleft",B_POINT_TYPE,0,(void *)&point, 8)) >= B_NO_ERROR)
		{
			MoveTo(point); // move to that point
			//point.PrintToStream();
		}
		//else
			//printf("bytes_read = %d. strerror(bytes_read) = %s\n", bytes_read, strerror(bytes_read));
	
	Show();
}

bool TickerWindow::QuitRequested()
{
	// save the position of the window's top-left corner.
	BFile file;
	BPath path;
	BPoint point = Frame().LeftTop();
	//ssize_t bytes_written=0;
	status_t err;
	
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("x-vnd.Prok-TheTick");
	path.Append("AppSettings");
	
	printf("path = %s\n", path.Path());
	
	file.SetTo(path.Path(), B_WRITE_ONLY | B_CREATE_FILE);
	if((err = file.InitCheck()) == B_OK)
		file.WriteAttr("topleft",B_POINT_TYPE,0,(void *)&point, 8);
	//else
		//printf("strerror(err) = %s\n", strerror(err));
	
	//point.PrintToStream();
	//printf("bytes_written = %d\n", bytes_written);
	
	// quit as usual
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}