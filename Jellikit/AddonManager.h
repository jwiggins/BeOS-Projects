/*

	AddonManager.h
	John Wiggins 1998

*/

#ifndef ADDON_MANAGER_H
#define ADDON_MANAGER_H

#include <Application.h>
#include <Looper.h>
#include <List.h>
#include <Path.h>
#include <Entry.h>
#include <Directory.h>
#include <image.h>
#include <string.h>

#include "MessageDefs.h"
#include "AddOnWrapper.h"

class AddonManager : public BLooper {

public:
						AddonManager();
						~AddonManager();
virtual thread_id		Run(); // load the addons and call inherited
virtual	void			MessageReceived(BMessage *msg);
image_id				GetAddonForType(type_code type);
private: 
	
BList *addon_list;
};

#endif