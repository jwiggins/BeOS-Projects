/*
	TickerAddon.h
	
	TheTick by John Wiggins. 1999

*/

#ifndef TICKER_ADDON_H
#define TICKER_ADDON_H

#include <SupportDefs.h>

struct TickerAddonInfo {
const char *SiteHost;
const char *SiteFile;
const char *SiteName;
int64 RefreshInterval;
};

//extern "C" 
//{
//	_EXPORT void parse(const char *buffer, int32 bufferlen, char ***output, char ***outlinks, int32 *outnum);
//	_EXPORT TickerAddonInfo Info = { "www.beoscentral.com", "/powerbosc.txt", "BeOS Central", (bigtime_t)30*60*1000*1000 /* 30 minutes */ };
//}

#endif