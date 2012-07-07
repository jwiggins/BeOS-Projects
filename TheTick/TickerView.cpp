/*
	TickerView.cpp
	
	TheTick by John Wiggins. 1999

*/

#include "TickerView.h"

TickerView::TickerView(BRect frame)
: BView(frame, "TheTick", B_FOLLOW_NONE, B_WILL_DRAW), ticker_speed(1),current_glyph_column(0), strings_have_changed(false), NewsItemCount(0), current_site(0), replicant(false)
{
	SetViewColor(B_TRANSPARENT_32_BIT); // This gets rid of flicker
	SetLowColor(0,0,0,255);
	LoadAddons(); // load add-ons first
	BuildPopUpMenu(); // the popup needs addon_list to have valid data
	
	
	frame.top = frame.bottom - 7.;
	frame.right = frame.left + 7.;
	AddChild(new BDragger(frame, this));
	
	tickerbit = new BBitmap(Bounds(), B_CMAP8, true);
	ticker = new Ticker(Bounds(), this);
	tickerbit->AddChild(ticker);
	
	pulsar = NULL;
	NewsList = NULL;
	LinksList = NULL;
}

// replicant ctor
TickerView::TickerView(BMessage *msg)
 : BView(msg), current_glyph_column(0), strings_have_changed(true), replicant(true)
{
	// debug
	//msg->PrintToStream();
	
	SetViewColor(B_TRANSPARENT_32_BIT); // This gets rid of flicker
	SetLowColor(0,0,0,255);
	LoadAddons(); // load add-ons first
	BuildPopUpMenu(); // the popup needs addon_list to have valid data
	
	tickerbit = new BBitmap(Bounds(), B_CMAP8, true);
	ticker = new Ticker(Bounds(), this);
	tickerbit->AddChild(ticker);
	
	pulsar = NULL;
	
	// data grabbin vars
	type_code type;
	const char *tmp_ptr;
	
	// grab the NewsList and LinksList if they exist
	NewsItemCount = 0;
	msg->GetInfo("news", &type, &NewsItemCount);
	
	if(NewsItemCount > 0)
	{
		// allocate
		NewsList = (char **)malloc( (NewsItemCount + 1) * sizeof(char *) );
		LinksList = (char **)malloc( (NewsItemCount + 1) * sizeof(char *) );
		
		memset((void *)NewsList, 0, ((NewsItemCount + 1) * sizeof(char *)));
		memset((void *)LinksList, 0, ((NewsItemCount + 1) * sizeof(char *)));
		
		for(int i=0;i<NewsItemCount;i++)
		{
			msg->FindString("news",i, &tmp_ptr);
			NewsList[i] = (char *)malloc(strlen(tmp_ptr)+1);
			strcpy(NewsList[i], tmp_ptr);
			tmp_ptr = NULL;
			
			msg->FindString("links",i, &tmp_ptr);
			LinksList[i] = (char *)malloc(strlen(tmp_ptr)+1);
			strcpy(LinksList[i], tmp_ptr);
			tmp_ptr = NULL;
		}
	}
	else
	{
		NewsItemCount = 0;
		NewsList = NULL;
		LinksList = NULL;
	}
	
	if(msg->FindInt32("speed", &ticker_speed ) < B_OK)
		ticker_speed = 1;
	if(msg->FindString("site", &tmp_ptr ) < B_OK)
		current_site = 0;
	else
	{
		AddOnWrapper *list_item = FindAddonByName(tmp_ptr);
		BMenuItem *menu_item = PopUpMenu->SubmenuAt(2)->FindItem(tmp_ptr);
		
		if(list_item != NULL)
		{
			//printf("list item for %s found.\n", tmp_ptr);
			current_site = list_item->Id();
		}
			
		if(menu_item != NULL)
			menu_item->SetMarked(true);		
	}
}

TickerView::~TickerView()
{
	// cleanup the offscreen
	delete tickerbit;
	// delete pulsar
	if(pulsar)
		delete pulsar;
	// delete the news items
	ClearStrings();
	// free the list of addons
	AddOnWrapper *list_item;
	int32 index=0;
	
	while((list_item = (AddOnWrapper *)addon_list->ItemAt(index++)) != NULL)
		delete list_item;
	
	delete addon_list;
	//printf("End of TickerView dtor.\n");
}

status_t TickerView::Archive(BMessage *msg, bool deep) const
{	
	msg->AddString("class", "TickerView");
  msg->AddString("add_on", APP_SIGNATURE);
    
  for(int i=0;i<NewsItemCount;i++)
	{
		msg->AddString("news", NewsList[i]);
		msg->AddString("links", LinksList[i]);
	}
	msg->AddString("site",PopUpMenu->SubmenuAt(2)->FindMarked()->Label());
	msg->AddInt32("speed",ticker_speed);
    
  BView::Archive(msg,deep);
    
  //printf("Archive(). deep = %s\n", deep ? "true" : "false");
	//msg->PrintToStream();
    
	return B_OK;
}

TickerView * TickerView::Instantiate(BMessage *msg)
{
	if(validate_instantiation(msg, "TickerView")) 
		return new TickerView(msg);

	return (TickerView *)NULL;
}

void TickerView::AboutRequested(void)
{
	(new BAlert("about", "TheTick v1.0 (replicant) by John Wiggins", "OK"))->Go(NULL);
}

void TickerView::AttachedToWindow(void)
{
	BView::AttachedToWindow();
	PopUpMenu->SetTargetForItems(this);
	PopUpMenu->SubmenuAt(1)->SetTargetForItems(this); // "Speed" menu
	PopUpMenu->SubmenuAt(2)->SetTargetForItems(this); // "Site" menu
	PopUpMenu->SubmenuAt(3)->SetTargetForItems(this); // "Links" menu
	((BMenuItem *)PopUpMenu->ItemAt(0))->SetTarget(be_app);
	
	if(replicant)
	{
		((BMenuItem *)PopUpMenu->ItemAt(0))->SetEnabled(false); // we don't want to kill our container app
		
		if(NewsItemCount > 0)
		{
			BMenu *menu = PopUpMenu->SubmenuAt(3);
	
			if(PopUpMenu->RemoveItem(menu))
			{
				delete menu;
				PopUpMenu->AddItem(new BMenu("Links"), 3);
			}
		
			for(int i=0;i<NewsItemCount;i++)
			{
				BMessage *a_msg = new BMessage(GOT_LINK);
				a_msg->AddInt32("id", i);
		
				PopUpMenu->SubmenuAt(3)->AddItem(new BMenuItem(NewsList[i], a_msg));
				a_msg = NULL;
			}
			PopUpMenu->SubmenuAt(3)->SetTargetForItems(this);
		}
	}
		
	
	// Oh, by the way... You have to be attached to a window for a BMessageRunner to be constructed correctly ...
	// caveat programmer
	if(!pulsar)
		pulsar = new BMessageRunner(BMessenger(this), new BMessage(TICKER_PULSE), 200000);
}

void TickerView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case TICKER_PULSE:
		{
			Pulse();
			break;
		}
		case SET_SPEED:
		{
			int32 the_speed;
			if(msg->FindInt32("to", &the_speed) == B_NO_ERROR)
				ticker_speed = the_speed;
			break;
		}
		case SET_SITE:
		{
			image_id addon;
			if(msg->FindInt32("id", &addon) == B_NO_ERROR)
				current_site = addon;
			break;
		}
		case GOT_LINK:
		{	
			int32 id;
			
			if(msg->FindInt32("id", &id) == B_NO_ERROR)
			{
				BMessenger postman("application/x-vnd.Be-NPOS");
				if(postman.IsValid())
				{
					BMessage open_url(B_NETPOSITIVE_OPEN_URL);
					
					open_url.AddString("be:url", LinksList[id]);
					postman.SendMessage(&open_url);
					//printf("Opening url...\n");
				}
				else
				{
					be_roster->Launch("text/html", 1, &LinksList[id]);
				}
			}
			break;
		}
		case GET_NEWS:
		{
			if(current_site != 0)
			{
				// SniffCache calls GotNews if the cached site is still valid and returns true
				if(!SniffCache(current_site))
				{
					if(find_thread("NewsGrabber") < 0)
					{
						//printf("spawning fire-and-forget thread\n");
						//printf("grabbing news with add-on %d\n", addon);
						thread_data_struct *data = (thread_data_struct *)malloc(sizeof(thread_data_struct));
						data->tickerview = this;
						data->addon = current_site;
					
						resume_thread(spawn_thread(GetNews_start, "NewsGrabber", B_NORMAL_PRIORITY, (void *)data));
					}
				}
			}
			break;
		}
		case GOT_NEWS:
		{
			//printf("got news.\n");
			char **the_news, **the_links;
			int32 num_items;
			
			if(msg->FindPointer("news", (void **)&the_news) == B_NO_ERROR)
				if(msg->FindPointer("links", (void **)&the_links) == B_NO_ERROR)
					if(msg->FindInt32("num", &num_items) == B_NO_ERROR)
						GotNews(the_news, the_links, num_items);
			//the_news = NULL;
			break;
		}
		case B_ABOUT_REQUESTED:
		{
			AboutRequested();
			break;
		}
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}

void TickerView::Draw(BRect update)
{
	BRegion region;
	BRect bounds = Bounds();
	
	GetClippingRegion(&region);
	bounds.top = bounds.bottom - 7.;
	bounds.right = bounds.left + 7.;
	region.Include(bounds);
	
	ConstrainClippingRegion(&region);
	DrawBitmap(tickerbit, update, update);
	ConstrainClippingRegion(NULL);
}

void TickerView::MouseDown(BPoint point)
{
	uint32 buttons = 0;
	
	GetMouse(&point, &buttons);
	if(buttons & B_SECONDARY_MOUSE_BUTTON)
	{
		ConvertToScreen(&point);
		PopUpMenu->Go(point, true, false, true);
	}
}

void TickerView::Pulse(void)
{
	if(strings_have_changed)
	{
		if(tickerbit->Lock())
		{
			BRect bounds(Bounds());
			ticker->clear(); // flush it
			tickerbit->Unlock();
			bounds.InsetBy(0,1);
			Invalidate(bounds);
			next_char(); // let next_char know 'strings_have_changed'.
			strings_have_changed = false;
		}
	}
	else if(tickerbit->Lock())
	{
		BRect bounds(Bounds());
		current_glyph_column = ticker->quasi_pulse(current_glyph_column, ticker_speed);
		tickerbit->Unlock();
		bounds.InsetBy(0,1);
		Invalidate(bounds);
	}
}

void TickerView::LoadAddons(void)
{
	//printf("LoadAddons() start.\n");
	// construct The List™ for "handy addon management" ®
	addon_list = new BList();
	
	// load all the add-ons and "wrap" them in joy
	image_id addonId;
	
	status_t err = B_NO_ERROR;
	int32 numentries=0;
	entry_ref ref;
	BPath path;
	
	//look in app directory for add-ons
	be_roster->FindApp(APP_SIGNATURE, &ref); // "replicant safe" ™®© :)
	BEntry entry(&ref);
	entry.GetPath(&path);
	//printf("LoadAddons(). %s\n", path.Path());
	path.GetParent(&path);
	path.Append("add-ons");
	
	BDirectory dir(path.Path());
	
	//printf("LoadAddons(). looking in %s\n", path.Path());
	
	if((err = dir.InitCheck()) < B_NO_ERROR)
	{
		//printf("The addon directory object is Bad!\n");
		return;// err;
	}
		
	numentries = dir.CountEntries();
	
	//load all add-ons
	while(err == B_NO_ERROR)
	{
		err = dir.GetNextEntry( (BEntry*)&entry, true );			
		if( entry.InitCheck() != B_NO_ERROR )
			break;
		if( entry.GetPath(&path) != B_NO_ERROR )
		{
			//printf("entry.GetPath failed\n");
		}
		else
		{
			addonId = load_add_on( path.Path() );
			if( addonId < 0 )
			{
				//printf("load_add_on( %s ) failed\n", path.Path() );
			}
			else
			{
				AddOnWrapper *add_on = new AddOnWrapper(addonId); // wrappers are phun :P
				if(add_on->InitCheck())
				{
					addon_list->AddItem((void *)add_on);
				}
			}
		}
	}
}

void TickerView::BuildPopUpMenu(void)
{
	BMessage *a_msg;
	AddOnWrapper *list_item = NULL;
	int32 index=0;
	
	PopUpMenu = new BPopUpMenu("TickerPopUp", false, false);
	
	PopUpMenu->AddItem(new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED)));
	PopUpMenu->AddItem(new BMenu("Speed"));
	
	a_msg = new BMessage(SET_SPEED);
	a_msg->AddInt32("to", 0);
	PopUpMenu->SubmenuAt(1)->AddItem(new BMenuItem("0", a_msg));
	
	a_msg = new BMessage(SET_SPEED);
	a_msg->AddInt32("to", 1);
	PopUpMenu->SubmenuAt(1)->AddItem(new BMenuItem("1", a_msg));
	
	a_msg = new BMessage(SET_SPEED);
	a_msg->AddInt32("to", 2);
	PopUpMenu->SubmenuAt(1)->AddItem(new BMenuItem("2", a_msg));
	
	a_msg = new BMessage(SET_SPEED);
	a_msg->AddInt32("to", 3);
	PopUpMenu->SubmenuAt(1)->AddItem(new BMenuItem("3", a_msg));
	
	a_msg = new BMessage(SET_SPEED);
	a_msg->AddInt32("to", 4);
	PopUpMenu->SubmenuAt(1)->AddItem(new BMenuItem("4", a_msg));
	
	a_msg = new BMessage(SET_SPEED);
	a_msg->AddInt32("to", 5);
	PopUpMenu->SubmenuAt(1)->AddItem(new BMenuItem("5", a_msg));
	PopUpMenu->SubmenuAt(1)->SetRadioMode(true);
	PopUpMenu->SubmenuAt(1)->ItemAt(1)->SetMarked(true);
	
	PopUpMenu->AddItem(new BMenu("Site"));
	while((list_item = (AddOnWrapper *)addon_list->ItemAt(index++)) != NULL)
	{		
		a_msg = new BMessage(SET_SITE);
		a_msg->AddInt32("id", list_item->Id());
		
		PopUpMenu->SubmenuAt(2)->AddItem(new BMenuItem(list_item->SiteName(), a_msg));
		a_msg = NULL;
	}
	PopUpMenu->SubmenuAt(2)->SetRadioMode(true);
	
	PopUpMenu->AddItem(new BMenu("Links"));
	
	PopUpMenu->AddItem(new BMenuItem("Grab News", new BMessage(GET_NEWS)));
	
	a_msg = NULL;
}

void TickerView::GotNews(char **the_news, char **the_links, int32 num_items)
{
	//printf("TickerView::GotNews()\n");
	
	if(NewsList != NULL)
		ClearStrings();
	NewsList = the_news; // point at the new stuff
	LinksList = the_links;
	NewsItemCount = num_items; // change to new value
	
	BMenu *menu = PopUpMenu->SubmenuAt(3);
	
	if(PopUpMenu->RemoveItem(menu))
	{
		delete menu;
		PopUpMenu->AddItem(new BMenu("Links"), 3);
	}
		
	for(int i=0;i<NewsItemCount;i++)
	{
		BMessage *a_msg = new BMessage(GOT_LINK);
		a_msg->AddInt32("id", i);
		
		PopUpMenu->SubmenuAt(3)->AddItem(new BMenuItem(NewsList[i], a_msg));
		a_msg = NULL;
	}
	PopUpMenu->SubmenuAt(3)->SetTargetForItems(this);
	
	//int32 i=0;
	//while(LinksList[i] != NULL)
	//{
	//	printf("LinksList[%d] = %s\n", i, LinksList[i]);
	//	i++;
	//}
		
	//printf("\nThat is all...\n");
	
	// Below lieth what happeneth when MALLOC_DEBUG sendeth the the programmer
	// into a tailspin... All ye abandon hope who cross this line ;P
	
	// make a copy. yuck. I f@#$ing hate memory :) (only on tuesdays .... )
	//NewsList = (char **)malloc(num_items + 1);
	//memset((void *)NewsList, 0, num_items);
	
	//for(int i=0;i<num_items;i++)
	//{
	//	int len=strlen(the_news[i]);
	//	NewsList[i] = (char *)malloc(len+1);
	//	memset((void *)NewsList[i], 0, len+1);
	//	strcpy(NewsList[i], the_news[i]);
	//}
	
	// then dispose of the cloned carcass
	//for(int i=0;i<NewsItemCount;i++)
	//{
		
	//	free(the_news[i]);
	//	the_news[i] = NULL;
	//}
	//free(the_news);
}

char TickerView::next_char(void)
{
	static int string_num=0, letter_num=0, color=0;
	static rgb_color colors[2];
	char the_char;
	
	colors[0].red=255;colors[0].green=255;colors[0].blue=255;colors[0].alpha=255;
	colors[1].red=192;colors[1].green=0;colors[1].blue=0;colors[1].alpha=255;

	//printf("TickerView::next_char() start. string_num = %d, letter_num = %d.\n", string_num, letter_num);
	// pre-assignment sanity check
	// There's just a NULL pointer at the end of the list. We dont wanna try and get chars from it. 
	if(string_num > (NewsItemCount-1))
	{
		string_num = 0; // reset this
		letter_num = 0; // this one too
	}
	
	// to avoid a race condition, next_char will be called from Pulse() whenever strings_have_changed
	// is true. When it is true, we need to reset string_num and letter_num.
	if(strings_have_changed)
	{
		string_num = 0; // reset this
		letter_num = 0; // this one too
	}
	else if(NewsList != NULL)
	{
		//printf("the_char = (NewsList[%d])[%d];\n", string_num, letter_num);
		the_char = (NewsList[string_num])[letter_num]; // assign ...
		
		// and inspect ...		
		if(the_char == '\0') // end of a string
		{
			the_char = ' '; // return a space
			letter_num = 0; // reset the letter_num
			string_num++;
			
			ticker->SetHighColor(colors[color]);
			if(color)
				color = 0;
			else
				color = 1;
		}
		else
			letter_num++;
	}
	else
		the_char = '\0';

	//printf("TickerView::next_char() end. string_num = %d, letter_num = %d.\n", string_num, letter_num);
	//printf("TickerView::next_char() returning %c\n", the_char);
	return (the_char);
}

void TickerView::ClearStrings(void)
{
	//printf("TickerView::ClearStrings() start\n");
	if(NewsList != NULL)
	{
		for(int i=0;i<NewsItemCount;i++)
		{
		//	printf("free(NewsList[%d]);\n", i);
			free(NewsList[i]);
			free(LinksList[i]);
			LinksList[i] = NULL;
			NewsList[i] = NULL;
		}
		//printf("free(NewsList);\n");
		free(LinksList);
	}
	NewsList = NULL;
	LinksList = NULL;
	NewsItemCount = 0;
	strings_have_changed = true;
	//printf("TickerView::ClearStrings() end\n");
}


/*
	Below here layeth the http code. GetNews() is handed to spawn_thread(). It calls ReadHTTPFile()
	and uses an add-on to parse the returned data, fater that. it sends a posts a message back to
	TickerView. then it dies a painful death

*/

int32 TickerView::GetNews_start(void *arg)
{
	//printf("GetNews_start()\n");
	thread_data_struct *data = (thread_data_struct *)arg;
	
	data->tickerview->GetNews(data->addon);
	
	free(data); // allocated when we were spawned. spawnee cannot free it, so we do it for them...
	return B_OK; // bogus return to satisfy compiler
}

void TickerView::GetNews(image_id addon)
{
	AddOnWrapper *news_addon;
	BMessage reply(GOT_NEWS);
	char *http_buffer = (char *)malloc(1024*32); // 32k buffer. yuck. lazyness
	char **the_news = NULL, **the_links = NULL;
	int32 outnum=0, http_bytes_read=0;
	
	news_addon = FindAddonById(addon);
	if(news_addon != NULL)
	{
		//printf("news_addon != NULL\n");
		
		memset((void *)http_buffer, 0, 32*1024);
		http_bytes_read = ReadHTTPFile(news_addon->SiteHost(),news_addon->SiteFile(), http_buffer, 1024*32);
		
		if(http_bytes_read > 0)
		{
			//printf("http_bytes_read = %d\n", http_bytes_read);
			//printf("the_news = %p\noutnum = %p\n", the_news, &outnum);
			//printf("%s\n", http_buffer);
			//http_buffer = NULL;
			
			// parse it!
			news_addon->parse(http_buffer, http_bytes_read, &the_news, &the_links, &outnum);
			
			//printf("the_news = %p\n", the_news);
			if((the_news != NULL) && (the_links != NULL))
			{
			
				//int32 i=0;
				//while(the_news[i] != NULL)
				//{
				//	printf("the_news[%d] = %s\n", i, the_news[i]);
				//	free(the_news[i]);
				//	the_news[i] = NULL;
				//	i++;
				//}
				//free(the_news);
			
				//printf("the_news = %p\noutnum = %p\n", the_news, &outnum);
			
				// nice version
				reply.AddPointer("news", (void *)the_news);
				reply.AddPointer("links", (void *)the_links);
				reply.AddInt32("num", outnum);
				BMessenger(this).SendMessage(&reply);
			}
			
			// cache the http buffer
			BFile file;
			BPath path;
			bigtime_t cachetime = real_time_clock_usecs();
			
			find_directory(B_USER_SETTINGS_DIRECTORY, &path);
			path.Append("x-vnd.Prok-TheTick");
			path.Append(news_addon->SiteName());
			
			file.SetTo(path.Path(), B_WRITE_ONLY|B_CREATE_FILE);
			if(file.InitCheck() == B_NO_ERROR)
			{
				file.WriteAt(0, (void *)&cachetime, sizeof(bigtime_t));
				file.WriteAt(sizeof(bigtime_t), (void *)http_buffer, http_bytes_read);
			}
		}
	}
	
	
	free(http_buffer); // clean up
	http_buffer = NULL;
}

bool TickerView::SniffCache(image_id addon)
{
	AddOnWrapper *site_addon = FindAddonById(addon);
	BFile file;
	BPath path;
	bigtime_t cached_time=0;
	bool retval;
	
	if(site_addon != NULL)
	{
		find_directory(B_USER_SETTINGS_DIRECTORY, &path);
		path.Append("x-vnd.Prok-TheTick");
		path.Append(site_addon->SiteName());
		
		//printf("path = %s\n", path.Path());
		
		file.SetTo(path.Path(), B_READ_ONLY);
		if(file.InitCheck() == B_NO_ERROR)
		{
			file.ReadAt(0, (void *)&cached_time, sizeof(bigtime_t));
			if((real_time_clock_usecs() - cached_time) > site_addon->RefreshRate())
			{
				// grab that news!
				retval = false; // returning false to MessageReceived will do that for us
			}
			else
			{
				// read in the cache from the file
				off_t size=0;
				char *buffer;
				char **the_news=NULL, **the_links=NULL;
				int32 outnum=0;
				
				file.GetSize(&size);
				size-=sizeof(bigtime_t); // account for the the bigtime_t at the front
				
				buffer = (char *)malloc(size); // buffer is written to file with NULL terminator
				
				if(file.ReadAt(sizeof(bigtime_t), (void *)buffer, size) == size )
				{
					site_addon->parse(buffer, size, &the_news, &the_links, &outnum);
					if(the_news != NULL && the_links != NULL)
						GotNews(the_news, the_links, outnum);
					retval = true;
				}
				else
					retval = false;
				
				free(buffer); // all done with this
				buffer = NULL;
			}
		}
		else
			retval = false; // file weren't thar. grab the news
	}
	else
		retval = false;
	
	return retval;
}

AddOnWrapper *TickerView::FindAddonById(image_id id)
{
	AddOnWrapper *list_item;
	int32 index=0;
	
	//printf("Finding add-on %d\n", id);
	while((list_item = (AddOnWrapper *)addon_list->ItemAt(index++)) != NULL)
	{
		if(list_item->Id() == id)
			break;
		else
			list_item = NULL;
	}
	
	return list_item;
}

AddOnWrapper *TickerView::FindAddonByName(const char *name)
{
	AddOnWrapper *list_item;
	int32 index=0;
	
	//printf("Finding add-on %s\n", name);
	while((list_item = (AddOnWrapper *)addon_list->ItemAt(index++)) != NULL)
	{
		if(!strcmp(list_item->SiteName(), name))
			break;
		else
			list_item = NULL;
	}
	
	return list_item;
}

int TickerView::ReadHTTPFile(const char *SiteHost, const char *URI, char *buffer, int size)
{
	sockaddr_in sa; /* Socket address */
	int tcp_socket; /* Socket itself */
	long host_addr; /* We need this to figure out the IP of SiteHost */
	hostent *theHost;
	int status; /* just a general purpose variable */
	
	char *buffer_start = buffer;
	int32 bytes_read=0;

	/* Create a stream TCP socket. */
	//printf("opening the socket.\n");
	if((tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		do_error("error: failed to create socket\n");
		return 0;
	}

	/* Set the address format for the imminent bind. */
	sa.sin_family = AF_INET;
	sa.sin_port = htons(80); // http
	//printf("doing the name lookup.\n");
	theHost = gethostbyname(SiteHost);
	
	if(theHost == NULL)
	{
		do_error("error: DNS lookup failed.\n");
		return 0;
	}

	sa.sin_addr.s_addr = *(ulong *)theHost->h_addr;

	/* Clear sin_zero. */
	memset(sa.sin_zero, 0, sizeof(sa.sin_zero));

	/* connect to the web server */
	//printf("connecting to the server...\n");
	status = connect(tcp_socket, (struct sockaddr *)&sa, sizeof(sa));

	if(status < 0)
	{
		do_error("error: failed to connect\n");
		closesocket(tcp_socket);

		printf("%s\n",strerror(status));
		return 0;
	}

	/* Build our request */
	/* HTTP 1.1 wants an endline then Host: <host> \r\n\r\n . Not much different than 1.0*/
	sprintf(buffer,"GET %s HTTP/1.1\nHost: %s\nUser-Agent: NewsTicker v1.0\r\n\r\n", URI, SiteHost);

	/* Send our request */
	//printf("sending our http request.\n");
	if((send(tcp_socket,buffer,((strlen(buffer))+1),0))<0)
	{
		do_error("error: failed to send\n");
		closesocket(tcp_socket);
		return 0;
	}
	
	//printf("http read loop.\n");
	do
	{
		status = has_tcp_data_come_in(tcp_socket,5);
		/* let's wait up to 5 seconds to see if we have any incoming data */
		if(status > 0)
		{
			status = recv(tcp_socket, buffer, size, 0);  /* grab the data! */
			if(status > 0)
			{
				bytes_read += status; // add to the bytes_read counter
				buffer += status;
			}
		}
	} while(status > 0 );
	//printf("\n");
	
	// cap off the buffer
	buffer_start[bytes_read] = '\0';
	bytes_read+=1; // compensate for the '\0' at the end.

	/* we no longer need our socket - so let's be nice and close it */
	//printf("closing the socket.\n");
	closesocket(tcp_socket);
	
	// tell em how much they won Bob!
	return bytes_read;
}

bool TickerView::has_tcp_data_come_in(int socket, int seconds)
{
	timeval tv;
	fd_set fds;
	int n;

	tv.tv_sec = seconds;
	tv.tv_usec = 0;

	/* Initialize (clear) the socket mask. */
	FD_ZERO(&fds);

	/* Set the socket in the mask. */
	FD_SET(socket, &fds);
	select(socket + 1, &fds, NULL, NULL, &tv);

	/* If the socket is still set, then it's ready to read. */
	return FD_ISSET(socket, &fds);
}

void TickerView::do_error(const char *error)
{
	(new BAlert("error", error, "ok"))->Go(NULL);
}