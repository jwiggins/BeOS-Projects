/*
	PluginManager.cpp
	2000 John Wiggins
*/

#include "PluginManager.h"

// ctor
PluginManager::PluginManager(thread_id src1, thread_id src2, BMessenger *pipeline)
: BLooper("Plugin Manager"), pluginDir(),
source1(src1), source2(src2),
imageLoadList(10), pluginLoadList()
{
	app_info info; 
	BPath path;
	
	// init all our BMessage data containers
	sources = new BMessage();
	mixers = new BMessage();
	filters = new BMessage();
	outputs = new BMessage();
	
	// cache the pointer to the graphics pipeline
	graphicsPipeline = pipeline;
	
	// locate the plugin directory
	be_app->GetAppInfo(&info);
	BEntry entry(&info.ref);
	entry.GetPath(&path);
	path.GetParent(&path);
	path.Append("add-ons");
	
	// assign pluginDir
	pluginDir = path.Path();
	
	// scan the plugin directory
	ScanPlugins();
	
	// ain't there yet
	defaultOutWin = NULL;
}

PluginManager::~PluginManager()
{
	delete sources;
	delete mixers;
	delete filters;
	delete outputs;
	delete graphicsPipeline;
	
	if (defaultOutWin != NULL)
		delete defaultOutWin;
}

void PluginManager::MessageReceived(BMessage *msg)
{
	//printf("PluginManager::MessageReceived()\n");
	//msg->PrintToStream();
	switch(msg->what)
	{
		case JE_LOAD_PLUGIN:
		{
			status_t err = B_OK;
			const char *name;
			int32 index, type;
			int32 which=-1;
			void *plugin = NULL;
			BMessage plugin_msg;
			
			void *window;
			
			
			if ((err = msg->FindString("name", &name)) == B_OK)
			{
				if ((err = msg->FindInt32("index", &index)) == B_OK)
				{
					if ((err = msg->FindInt32("type", &type)) == B_OK)
					{
						switch(type)
						{
							case JE_SOURCE_PLUGIN:
							{
								plugin_msg.what = JE_SOURCE_CHANGED;
								if ((err = msg->FindInt32("which", &which)) == B_OK)
								{
									if ((err = msg->FindPointer("window", &window)) == B_OK)
									{
										je_source_args args;
										args.window = new BMessenger(reinterpret_cast<BHandler *>(window));
										args.graphics_pipeline = new BMessenger(*graphicsPipeline);
										args.id = which;
										err = LoadPlugin(name, index, (void *)&args, &plugin);
									}
								}
								
								break;
							}
							case JE_MIXER_PLUGIN:
							{
								plugin_msg.what = JE_MIXER_CHANGED;
								err = LoadPlugin(name, index, NULL, &plugin);
								break;
							}
							case JE_FILTER_PLUGIN:
							{
								plugin_msg.what = JE_FILTER_ADDED;
								if ((err = msg->FindInt32("which", &which)) == B_OK)
								{
									err = LoadPlugin(name, index, NULL, &plugin);
								}
								break;
							}
							case JE_OUTPUT_PLUGIN:
							{
								plugin_msg.what = JE_OUTPUT_CHANGED;
								err = LoadPlugin(name, index, NULL, &plugin);
								break;
							}
							default:
							{
								// shouldn't get here
								break;
							}
						}
						
						// now send the plugin to its recipient
						if (graphicsPipeline->IsValid())
						{
							plugin_msg.AddPointer("plugin", plugin);
							plugin_msg.AddInt32("which", which);
							err = graphicsPipeline->SendMessage(&plugin_msg);
						}
						else
							err = B_ERROR;
						
						// ahh, but if it's a source....
						if (type == JE_SOURCE_PLUGIN)
						{
							// sources aren't loopers
							// use send_data() with them
							if (which == 0)
								err = send_data(source1, 0, &plugin, sizeof(plugin));
							else if (which == 1)
								err = send_data(source2, 0, &plugin, sizeof(plugin));
						}
						
						// and finally, tell be_app that it's loaded
						if (err == B_OK)
						{
							// "re-use" msg
							msg->what = JE_PLUGIN_LOADED;
							
							// remove unwanted fields
							// "desc", "type", and "which" remain
							msg->RemoveName("window");
							msg->RemoveName("index");
							msg->RemoveName("name");
							
							be_app->PostMessage(msg);
						}
					}
				}
			}
			
			//printf("PluginManager::MessageRecieved(JE_LOAD_PLUGIN): %s\n", strerror(err));
			break;
		}
		case JE_UNLOAD_PLUGIN:
		{
			//printf("PluginManager::MessageRecieved(JE_UNLOAD_PLUGIN)\n");
			status_t err = B_OK;
			void *plugin;
			int32 type;
			BMessage reply, copy(*msg);
			
			// first, relay the message to the graphics pipeline
			// so that it can stop using the plugin we are about
			// to unload. Block on the reply.
			if (graphicsPipeline->IsValid())
				err = graphicsPipeline->SendMessage(&copy, &reply);
			
			if ((err = msg->FindPointer("plugin", &plugin)) == B_OK && plugin != NULL)
			{
				if ((err = msg->FindInt32("type", &type)) == B_OK)
				{
					switch(type)
					{
						case JE_SOURCE_PLUGIN:
						{
							je_source_plugin *the_plugin = (je_source_plugin *)plugin;
							
							// mmmm Source plugins...
							// teardown is a little shitty in the case of sources
							
							// first, we exit the source
							the_plugin->exit(the_plugin);
							
							// free the data allocated by the plugin
							the_plugin->free(the_plugin);
							
							// then free the plugin
							delete the_plugin;
							break;
						}
						case JE_MIXER_PLUGIN:
						{
							je_mixer_plugin *the_plugin = (je_mixer_plugin *)plugin;
							
							// free the data allocated by the plugin
							the_plugin->free(the_plugin);
							
							// then free the plugin
							delete the_plugin;
							break;
						}
						case JE_FILTER_PLUGIN:
						{
							je_filter_plugin *the_plugin = (je_filter_plugin *)plugin;
							
							// free the data allocated by the plugin
							the_plugin->free(the_plugin);
							
							// then free the plugin
							delete the_plugin;
							break;
						}
						case JE_OUTPUT_PLUGIN:
						{
							je_output_plugin *the_plugin = (je_output_plugin *)plugin;
							
							// free the data allocated by the plugin
							the_plugin->free(the_plugin);
							
							// then free the plugin
							delete the_plugin;
							break;
						}
						default:
						{
							// shouldn't get here
							break;
						}
					}
					
					// after cleaning up the plugin instance itself, we
					// unload the instance. Easy.
					UnloadPlugin(plugin);
					
					// then we check to see if the mixer died
					// if the mixer died, default out goes back to color bars
					if (type == JE_MIXER_PLUGIN && defaultOutWin != NULL)
					{
						BMessage draw(JE_DRAW_BITMAP);
						draw.AddPointer("bitmap", NULL);
						defaultOutWin->SendMessage(&draw);
					}
				}
			}
			
			//printf("PluginManager::MessageRecieved(JE_UNLOAD_PLUGIN): %s\n", strerror(err));
			break;
		}
		case JE_GET_AVAILABLE_PLUGINS:
		{
			//printf("PluginManager::MessageReceived(JE_GET_AVAILABLE_PLUGINS)\n");
			// tell the UI what we got!
			BMessage reply(JE_GET_AVAILABLE_PLUGINS);
			
			// add the relevant BMessage data containers
			reply.AddPointer("sources", sources);
			reply.AddPointer("mixers", mixers);
			reply.AddPointer("filters", filters);
			reply.AddPointer("outputs", outputs);
			
			// reply
			msg->SendReply(&reply);
			
			break;
		}
		case JE_SET_DEFAULT_OUTPUT:
		{
			void *window;
			
			if (msg->FindPointer("window", &window) == B_OK)
			{
				// only if default output is NULL
				if (defaultOutWin == NULL)
				{
					// default output
					defaultOutWin = new BMessenger(reinterpret_cast<BHandler *>(window));
					if (!defaultOutWin->IsValid())
					{
						delete defaultOutWin;
						defaultOutWin = NULL;
					}
				}
			}
			break;
		}		
		default:
		{
			BLooper::MessageReceived(msg);
			break;
		}
	}
}

// load a plugin
// - path is the add-on dir relative path of the image containing the plugin
// - index is the list index of the plugin
// - inst is the returned plugin instance
// * may modify imageLoadList
status_t PluginManager::LoadPlugin(const char *path, int32 index, void *args, void **inst)
{
	image_item *item;
	status_t err = B_OK;
	
	// load the plugin if needed
	if (!IsLoaded(path))
	{
		image_id addn_id;
		BPath file(pluginDir.String());
		
		file.Append(path);
		addn_id = load_add_on(file.Path());
		if (addn_id < 0)
		{
			printf( "LoadPlugin(): load_add_on( %s ) failed\n", file.Path() );
			err = addn_id;
		}
		else
		{
			// we've a new image loaded... make note of it
			item = new image_item;
			item->id = addn_id;
			item->name = new char [strlen(path)+1];
			strcpy(item->name, path);
			item->refcount = 0; // not yet
			
			imageLoadList.AddItem((void *)item);
			err = B_OK; // the image is loaded
		}
	}
	
	// then find the plugin image and instantiate the plugin
	if (err == B_OK) // the plugin code will be there
	{
		int32 i=0;
		while ((item = (image_item *)imageLoadList.ItemAt(i)) != NULL)
		{
			// is this the one?
			if (!strcmp(item->name, path))
			{
				je_plugin **(*get_plugins)(void);
				je_plugin **list;
				
				err = get_image_symbol(item->id, "get_plugins", B_SYMBOL_TYPE_TEXT, (void **)&get_plugins);
				
				if (err == B_OK)
				{
					// get the list
					list = get_plugins();
					// instantiate the plugin
					err = InstantiatePlugin(path, list[index], args, inst);
					if (err == B_OK)
						item->refcount++; // increment the reference count
				}
				
				// and we're done after that
				break;
			}
			i++;
		}
	}
	
	return err;
}

// create an instance of a plugin
// - addn is a pointer to the plugin
// - args is the data being passed to the plugin
// - inst is the returned instance
// * will modify pluginLoadList
status_t PluginManager::InstantiatePlugin(const char *path, je_plugin *addn, void *args, void **inst)
{
	status_t err = B_ERROR;
	
	// quick sanity check
	if (addn != NULL)
	{
		// do the instantiation
		*inst = addn->instantiate(args);
		
		if (*inst != NULL) // we are valid
		{
			// add this instance to pluginLoadList
			plugin_item *item = new plugin_item;
			item->instance = *inst;
			item->name = new char [strlen(path)+1];
			strcpy(item->name, path);
			
			pluginLoadList.AddItem((void *)item);
			
			//good instantiation (we assume :)
			err = B_OK;
		}
		else // ruh roh. bad instantiation
			err = B_ERROR;
	}
	
	return err;
}

// cheak to see if a plugin image is loaded
bool PluginManager::IsLoaded(const char *path)
{
	bool ret = false;
	image_item *item;
	int32 i=0;
	
	// loop through the list of loaded images
	while ((item = (image_item *)imageLoadList.ItemAt(i)) != NULL)
	{
		if (!strcmp(item->name, path))
		{
			// found it!
			ret = true;
			break;
		}
		i++;
	}
	
	return ret;
}

// unload a plugin
// - inst is a pointer to an instance of the plugin
// * THE PLUGIN INSTANCE WILL NOT HAVE ITS MEMORY FREE'D BY THIS FUNCTION
// * may modify imageLoadList
// * will modify pluginLoadList
void PluginManager::UnloadPlugin(void *inst)
{
	plugin_item *p_item;
	image_item *i_item;
	int32 i=0;
	
	// look for this instance
	while ((p_item = (plugin_item *)pluginLoadList.ItemAt(i)) != NULL)
	{
		if (p_item->instance == inst) // found it
		{
			// remove it from the list
			p_item = (plugin_item *)pluginLoadList.RemoveItem(i);
			
			break; // done
		}
		i++;
	}
	
	if (p_item != NULL)
	{
		i=0;
		
		// first decrement the refcount of the plugin image
		while ((i_item = (image_item *)imageLoadList.ItemAt(i)) != NULL)
		{
			if (!strcmp(i_item->name, p_item->name)) // found it
			{
				// decrement the refcount
				i_item->refcount--;
				// clean up unreferenced images
				if (i_item->refcount == 0)
				{
					i_item = (image_item *)imageLoadList.RemoveItem(i); // out of the list
					unload_add_on(i_item->id); // get rid of the image
					delete [] i_item->name; // delete the name
					delete i_item; // delete the item
				}
				
				break; // done
			}
			i++;
		}
		
		// then delete the plugin item
		delete p_item->name;
		delete p_item;
	}
	else // oh shit (p_item == NULL)
		; // that means we've lost track of a plugin.. mem leak bad!
}

// searches the add-on directory for available plugins and
// caches information about found plugins
void PluginManager::ScanPlugins()
{
	status_t err = B_NO_ERROR;
	int32 numentries=0;
	image_id addonId;
	BEntry entry;
	BPath path;
	BDirectory dir(pluginDir.String());
	
	if((err = dir.InitCheck()) < B_NO_ERROR)
	{
		printf("ScanPlugins(): The addon directory object is Bad!\n");
	}
	else
	{
		numentries = dir.CountEntries();
		
		// load all add-ons
		while(err == B_NO_ERROR)
		{
			err = dir.GetNextEntry(&entry, true );			
			if( entry.InitCheck() != B_NO_ERROR )
				break;
			if( entry.GetPath(&path) != B_NO_ERROR )
			{
				printf( "ScanPlugins(): entry.GetPath failed\n" );
			}
			else
			{
				addonId = load_add_on( path.Path() );
				if( addonId < 0 )
				{
					printf( "ScanPlugins(): load_add_on( %s ) failed\n", path.Path() );
				}
				else
				{
					// the add-on loaded successfully
					je_plugin **(*get_plugins)(void);
					je_plugin **list;
					err = get_image_symbol(addonId, "get_plugins", B_SYMBOL_TYPE_TEXT, (void **)&get_plugins);
					
					if (err == B_OK)
					{
						// grab the list from the add-on
						list = get_plugins();
						DiscoverPlugins(list, path.Leaf());
					}
					else
						printf("ScanPlugins(): get_image_symbol() failed\n");
					
					unload_add_on(addonId);
				}
			}
		}
	}
}

// implements actual cacheing of plugin info
void PluginManager::DiscoverPlugins(je_plugin **list, const char *image)
{
	//printf("DiscoverPlugins(): image = %s\n", image);
	int32 index=0;
	while (list[index] != NULL)
	{
		switch(list[index]->type)
		{
			case JE_SOURCE_PLUGIN:
			{
				sources->AddString("name", image);
				sources->AddString("desc", list[index]->short_desc);
				sources->AddInt32("index", index);
				break;
			}
			case JE_MIXER_PLUGIN:
			{
				mixers->AddString("name", image);
				mixers->AddString("desc", list[index]->short_desc);
				mixers->AddInt32("index", index);
				break;
			}
			case JE_FILTER_PLUGIN:
			{
				filters->AddString("name", image);
				filters->AddString("desc", list[index]->short_desc);
				filters->AddInt32("index", index);
				break;
			}
			case JE_OUTPUT_PLUGIN:
			{
				outputs->AddString("name", image);
				outputs->AddString("desc", list[index]->short_desc);
				outputs->AddInt32("index", index);
				break;
			}
			default:
			{
				break;
			}
		}
		
		index++; // next!
	}
	
	//sources->PrintToStream();
	//mixers->PrintToStream();
	//filters->PrintToStream();
	//outputs->PrintToStream();
}