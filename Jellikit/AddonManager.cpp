/*

	AddonManager.cpp - manage them add-ons !
	John Wiggins 1998

*/

#include "AddonManager.h"

// BLoopers and Practical jokes!
AddonManager::AddonManager()
			: BLooper("Addon Manager", B_NORMAL_PRIORITY, B_LOOPER_PORT_DEFAULT_CAPACITY)
{
	// construct The List™ for "handy addon management" ®
	addon_list = new BList();
	
	// load all the add-ons and "wrap" them in joy
	image_id addonId;
	
	status_t err = B_NO_ERROR;
	int32 numentries=0;
	app_info info; 
	BPath path;
	
	//look in app directory for add-ons
	be_app->GetAppInfo(&info);
	BEntry entry(&info.ref);
	entry.GetPath(&path);
	path.GetParent(&path);
	path.Append("add-ons");
	
	BDirectory dir(path.Path());
	
	if((err = dir.InitCheck()) < B_NO_ERROR)
	{
		printf("The addon directory object is Bad!\n");
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
			printf( "entry.GetPath failed\n" );
		}
		else
		{
			addonId = load_add_on( path.Path() );
			if( addonId < 0 )
			{
				printf( "load_add_on( %s ) failed\n", path.Path() );
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

AddonManager::~AddonManager()
{
	void *anItem;
	for(int32 i=0; (anItem = addon_list->ItemAt(i)) != NULL; i++)
		delete anItem;
	delete addon_list;
}

thread_id AddonManager::Run()
{
	//printf("AddonManager::Run() : BLooper::Run();\n");
	return BLooper::Run();
}

void AddonManager::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case DO_YOU_SUPPORT_THIS_TYPE:
		{
			//printf("AddonManager::MessageReceived(). DO_YOU_SUPPORT_THIS_TYPE\n");
			type_code typecode=0;
			image_id return_id;
			if(msg->FindInt32("typecode", (int32 *)&typecode) == B_NO_ERROR)
			{
				//printf("AddonManager::MessageReceived(). typecode was retrieved from message. type=%d\n",typecode);
				//printf("AddonManager::MessageReceived(). strerror(typecode)=%d\n", strerror(typecode));
				if((return_id = GetAddonForType(typecode)) > 0)
				{
					// yes
					BMessage reply(ANSWER_YES);
					reply.AddInt32("id", return_id);
					msg->SendReply(&reply);
					//printf("AddonManager::MessageReceived(). returning positive reply\n");
				}
				else
				{
					// no
					msg->SendReply(ANSWER_NO);
					//printf("AddonManager::MessageReceived(). returning negative reply\n");
				}
			}
			else // there was an error, so tell them you don't support it to be safe
				msg->SendReply(ANSWER_NO);
			break;
		}
		default:
			BLooper::MessageReceived(msg);
			break;
	}
}

image_id
AddonManager::GetAddonForType(type_code type)
{
	//printf("AddonManager::GetAddonForType()\n");
	int32 i=0;
	AddOnWrapper *list_item;
	while((list_item = (AddOnWrapper *)addon_list->ItemAt(i++)) != NULL)
	{
		//printf("AddonManager::GetAddonForType(). while loop.\n");
		if(list_item->SupportsType(&type))
		{
			return(list_item->Id());
		}
	}
	return(-0x7f000000);
}