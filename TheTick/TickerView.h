/*
	TickerView.h
	
	TheTick by John Wiggins. 1999

*/

#ifndef TICKER_VIEW_H
#define TICKER_VIEW_H

#include <Application.h>
#include <MessageRunner.h>
#include <Messenger.h>
#include <Roster.h>

#include <Alert.h>
#include <View.h>
#include <Bitmap.h>
#include <PopUpMenu.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Dragger.h>
#include <Region.h>

#include <List.h>
#include <Archivable.h>

#include <OS.h>

#include <Path.h>
#include <Entry.h>
#include <Directory.h>
#include <FindDirectory.h>

#include <be_apps/NetPositive/NetPositive.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <net/socket.h>
#include <net/netdb.h>

#include "Ticker.h"
#include "AddOnWrapper.h"

#define APP_SIGNATURE "application/x-vnd.Prok-TheTick"

class Ticker;
class AddOnWrapper;


class _EXPORT TickerView : public BView {
public:
_EXPORT 					TickerView(BRect frame);
_EXPORT 					TickerView(BMessage *msg); // replicant related
_EXPORT 					~TickerView();
_EXPORT virtual status_t		Archive(BMessage *msg, bool deep) const; // replicant related
_EXPORT static TickerView *	Instantiate(BMessage *msg); // replicant related
_EXPORT virtual void			AboutRequested(void); // replicant related

_EXPORT virtual void		AttachedToWindow(void);
_EXPORT virtual void		MessageReceived(BMessage *msg);
_EXPORT virtual void		Draw(BRect);
_EXPORT virtual void		MouseDown(BPoint point);
_EXPORT virtual void		Pulse(void);

private:
friend class				Ticker; // Ticker needs next_char()
_EXPORT void				LoadAddons(void);
_EXPORT void				BuildPopUpMenu(void);
_EXPORT void				GotNews(char **the_news, char **the_links, int32 num_items);
_EXPORT void				ClearStrings(void);
_EXPORT char				next_char(void);

_EXPORT static int32		GetNews_start(void *arg); // fire-and-forget thread
_EXPORT void				GetNews(image_id addon);
_EXPORT bool				SniffCache(image_id addon);
_EXPORT AddOnWrapper *	FindAddonById(image_id id);
_EXPORT AddOnWrapper *	FindAddonByName(const char *name);
_EXPORT int				ReadHTTPFile(const char *, const char *, char *, int);
_EXPORT bool				has_tcp_data_come_in(int socket, int seconds);
_EXPORT void				do_error(const char *error);

BMessageRunner		*pulsar;
BPopUpMenu			*PopUpMenu;
BBitmap				*tickerbit;
Ticker				*ticker;
BList					*addon_list;
char					**NewsList, **LinksList;
int32					NewsItemCount, ticker_speed, current_glyph_column;
bool					strings_have_changed, replicant;
image_id			current_site;
};

struct thread_data_struct {
	TickerView *tickerview;
	image_id addon;
};

#endif