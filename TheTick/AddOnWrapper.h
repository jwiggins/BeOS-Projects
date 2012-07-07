/*
	AddOnWrapper.h - simple class to make dealing with add-ons a bit easier
	
	TheTick by John Wiggins. 1999

*/

#ifndef ADDON_WRAPPER_H
#define ADDON_WRAPPER_H

#include <SupportDefs.h>
#include <image.h>
#include <stdio.h>

#include "TickerAddon.h"

class _EXPORT AddOnWrapper {
	
public:
_EXPORT					AddOnWrapper(image_id id);
_EXPORT					~AddOnWrapper();
_EXPORT void					parse(const char *, int32, char ***, char ***, int32 *);
bool					InitCheck() const { return initialized; } // tells us if all the symbols loaded ok
image_id				Id() const { return addon_id; }
const char				*SiteName() const { return addon_info->SiteName; }
const char				*SiteHost() const { return addon_info->SiteHost; }
const char				*SiteFile() const { return addon_info->SiteFile; }
bigtime_t				RefreshRate() const { return addon_info->RefreshInterval; }

private:
image_id			addon_id;
bool					initialized;
TickerAddonInfo		*addon_info;
void					(*parse_func)(const char *, int32, char ***, char ***, int32 *);

};

#endif