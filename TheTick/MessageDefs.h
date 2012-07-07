/*
	MessageDefs.h - BMessage 'what' constants for NewsTicker
	
	TheTick by John Wiggins. 1999

*/

#ifndef MESSAGE_DEFS_H
#define MESSAGE_DEFS_H

enum {
GET_NEWS				= 'getn', /* request for news */
GOT_NEWS				= 'gotn', /* response to GET_NEWS */
SET_SITE				= 'site',
GOT_LINK				= 'link',
TICKER_PULSE			= 'tick',
SET_SPEED				= 'sspd',
SET_COLORS				= 'scol'
};

#endif