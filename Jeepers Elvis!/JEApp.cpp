/*
	JEApp.cpp
	2000 John Wiggins
*/

#include "JEApp.h"

JEApp::JEApp(const char *sig)
: BApplication(sig)
{
	thread_id thid1, thid2;
	
	// construct the two sources
	source1 = new SourceThread("Source0", &thid1);
	source2 = new SourceThread("Source1", &thid2);
	
	// might as well just fail if these are bad
	assert(thid1 > 0 && thid2 > 0);
	
	// now construct the graphics pipeline
	graphPipe = new GraphicsPipeline();
	graphPipe->Run();
	
	// make a plugin manager
	piManager = new PluginManager(thid1, thid2, new BMessenger((BHandler *)graphPipe));
	piManager->Run();
	
	// these will be taken care of in ReadyToRun()
	source1Win = NULL;
	source2Win = NULL;
	defaultOut = NULL;
	mainWin = NULL;
	
	// no windows yet
	windowCount = 0;
	
	// haven't told everyone to quit yet
	postedQuit = false;
}

JEApp::~JEApp()
{
	//printf("JEApp::~JEApp()\n");
	// our two sources
	delete source1;
	delete source2;
	
	// chill
	snooze(5 * 1000);
	
	// lock, quit 'n delete
	graphPipe->Lock();
	graphPipe->Quit();
	delete graphPipe;
	
	// chill
	snooze(5 * 1000);
	
	// lock, quit 'n delete
	piManager->Lock();
	piManager->Quit();
	delete piManager;
}

void JEApp::ReadyToRun()
{
	// instantiate UI here
	
	// preload ICON resources used by UI
	AppResources()->PreloadResourceType('ICON');
	
	// Window for Source1
	source1Win = new VideoWindow(BRect(10.0,75.0, 179.0,204.0), ResString(STRING_SOURCE1), true);
	
	// Window for Source2
	source2Win = new VideoWindow(BRect(10.0,280.0, 179.0,409.0), ResString(STRING_SOURCE2), true);
	
	// Window for default Output
	defaultOut = new VideoWindow(BRect(510.0,75.0, 839.0,324.0), ResString(STRING_DEFAULT_OUTPUT), false);
	
	// Window for Plugins
	piWindow = new PluginListWindow(BRect(510.0,375.0, 689.0,554.0), ResString(STRING_PLUGINS));
	
	// Main Window
	mainWin = new MainWindow(BRect(200.0,75.0, 499.0,474.0), "Jeepers Elvis!");
	
	// you need to tell the graphics pipeline about the default output
	PostMessage(JE_SET_DEFAULT_OUTPUT);
	// and the sources about their outputs
	PostMessage(JE_SET_SOURCE_OUTPUTS);
}

void JEApp::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case JE_LOAD_PLUGIN:
		{
			int32 type;
			int32 which;
			
			if (msg->FindInt32("type", &type) == B_OK)
			{
				if (msg->FindInt32("which", &which) == B_OK)
				{
					if (type == JE_SOURCE_PLUGIN)
					{
						if (which == 0)
							msg->AddPointer("window", source1Win);
						else
							msg->AddPointer("window", source2Win);
					}
					
					// tell the plugin manager
					piManager->PostMessage(msg);
				}
			}
			
			break;
		}
		case JE_UNLOAD_PLUGIN:
		{
			int32 type;
			int32 which;
			void *plugin;
			
			if (msg->FindInt32("type", &type) == B_OK)
			{
				if (msg->FindInt32("which", &which) == B_OK)
				{
					switch(type)
					{
						case JE_SOURCE_PLUGIN:
						{
							plugin = graphPipe->GetSource(which);
							break;
						}
						case JE_MIXER_PLUGIN:
						{
							plugin = graphPipe->GetMixer();
							break;
						}
						case JE_FILTER_PLUGIN:
						{
							plugin = graphPipe->GetFilter(which);
							break;
						}
						case JE_OUTPUT_PLUGIN:
						{
							plugin = graphPipe->GetOutput();
							break;
						}
						default:
						{
							plugin = NULL;
							break;
						}
					}
					
					// add the plugin
					msg->AddPointer("plugin", plugin);
					// remove the which
					msg->RemoveName("which");
					// tell the plugin manager
					piManager->PostMessage(msg);
				}
			}
			
			break;
		}
		case JE_PLUGIN_LOADED:
		{
			// a plugin has been loaded.
			// notify the GUI
			mainWin->PostMessage(msg);
			break;
		}
		case JE_WINDOW_REGISTRY_ADD:
		{
			// window spawning
			windowCount++;
			break;
		}
		case JE_WINDOW_REGISTRY_SUB:
		{
			// window dying
			windowCount--;
			if (windowCount == 0)
				PostMessage(B_QUIT_REQUESTED);
			break;
		}
		case JE_START_SOURCE:
		{
			int32 which;
			
			if (msg->FindInt32("which", &which) == B_OK)
			{
				if (which == 0)
					source1->Start();
				else
					source2->Start();
			}
			
			break;
		}
		case JE_STOP_SOURCE:
		{
			int32 which;
			
			if (msg->FindInt32("which", &which) == B_OK)
			{
				if (which == 0)
					source1->Stop();
				else
					source2->Stop();
			}
			
			break;
		}
		case JE_GET_UI:
		{
			int32 type;
			int32 which;
			void *view = NULL;
			BMessage reply(B_REPLY);
			
			if (msg->FindInt32("type", &type) == B_OK)
			{
				if (msg->FindInt32("which", &which) == B_OK)
				{
					switch(type)
					{
						case JE_SOURCE_PLUGIN:
						{
							je_source_plugin *plugin;
							plugin = (je_source_plugin *)graphPipe->GetSource(which);
							
							if (plugin != NULL)
								view = (void *)plugin->getUI(plugin);
							else
								view = NULL;
							break;
						}
						case JE_MIXER_PLUGIN:
						{
							je_mixer_plugin *plugin;
							plugin = (je_mixer_plugin *)graphPipe->GetMixer();
							
							if (plugin != NULL)
								view = (void *)plugin->getUI(plugin);
							else
								view = NULL;
							break;
						}
						case JE_FILTER_PLUGIN:
						{
							je_filter_plugin *plugin;
							plugin = (je_filter_plugin *)graphPipe->GetFilter(which);
							
							if (plugin != NULL)
								view = (void *)plugin->getUI(plugin);
							else
								view = NULL;
							break;
						}
						case JE_OUTPUT_PLUGIN:
						{
							je_output_plugin *plugin;
							plugin = (je_output_plugin *)graphPipe->GetOutput();
							
							if (plugin != NULL)
								view = (void *)plugin->getUI(plugin);
							else
								view = NULL;
							break;
						}
						default:
							break;
					}
				}
			}
			
			// add the view pointer to the reply msg
			reply.AddPointer("view", view);
			//printf("JEApp::MessageReceived(JE_GET_UI) : view = %p\n", view);
			//reply.PrintToStream();
			// send the reply
			msg->SendReply(&reply);
			
			break;
		}
		case JE_GET_AVAILABLE_PLUGINS:
		{
			//printf("JEApp::MessageReceived(JE_GET_AVAILABLE_PLUGINS)\n");
			BMessenger piMessenger(piManager);
			BMessage reply(B_REPLY);
			
			if (piMessenger.IsValid())
			{
				status_t err = piMessenger.SendMessage(JE_GET_AVAILABLE_PLUGINS, &reply);
				//printf("strerror(err) = %s\n", strerror(err));
				//printf("reply.what = %d\n", reply.what);
				err = msg->SendReply(&reply);
				//printf("strerror(err) = %s\n", strerror(err));
			}
			else
			{
				printf("piMessenger is invalid\n");
			}
			
			break;
		}
		case JE_MOVE_FILTER:
		{
			// simple relay
			graphPipe->PostMessage(msg);
			
			break;
		}
		case JE_SET_DEFAULT_OUTPUT:
		{
			// tell the Graphics Pipeline about the default output
			// add a pointer to the default output
			msg->AddPointer("window", defaultOut);
			// relay to graphPipe
			graphPipe->PostMessage(msg);
			// and relay to piManager
			piManager->PostMessage(msg);
			
			break;
		}
		case JE_UNSET_DEFAULT_OUTPUT:
		{
			// simple relay to graphPipe
			graphPipe->PostMessage(msg);
			
			break;
		}
		case JE_SET_SOURCE_OUTPUTS:
		{
			// tell the sources where their windows are
			source1->SetOutputWindow(source1Win);
			source2->SetOutputWindow(source2Win);
			
			break;
		}
		case JE_SEND_EMAIL:
		{
			int32 which;
			
			if (msg->FindInt32("which", &which) == B_OK)
			{
				// send email button was clicked
				if (which == 0)
					SendMeEmail(); // woohoo, feedback!
			}
			break;
		}
		default:
		{
			BApplication::MessageReceived(msg);
			break;
		}
	}
}

void JEApp::AboutRequested()
{
	BAlert *alert;
	BString gutsText("Jeepers Elvis! v0.2\n");
	gutsText += B_UTF8_COPYRIGHT;
	gutsText += " John Wiggins : 2000-2002\n\n- ";
	gutsText += RandomQuote();
	
	alert = new BAlert("about", gutsText.String(),
			ResString(STRING_EMAIL), ResString(STRING_OK));
	alert->Go(new BInvoker(new BMessage(JE_SEND_EMAIL), this));
}

bool JEApp::QuitRequested()
{
	//printf("JEApp::QuitRequested()\n");
	
	if (!postedQuit)
	{
		// tell all the windows to DIE :)
		source1Win->PostMessage(JE_APP_IS_QUITTING);
		source2Win->PostMessage(JE_APP_IS_QUITTING);
		defaultOut->PostMessage(JE_APP_IS_QUITTING);
		piWindow->PostMessage(JE_APP_IS_QUITTING);
		mainWin->PostMessage(JE_APP_IS_QUITTING);
		
		// we've told them to quit
		postedQuit = true;
	}
	
	// don't quit until all our windows are dead
	if (windowCount == 0)
		return true;
	else
		return false;
}

void JEApp::SendMeEmail()
{
	//printf("You've got mail!\n");
	const char *argv[3]= { "mailto:prok@mail.utexas.edu", "-subject", "Jeepers Elvis!" };
	be_roster->Launch("text/x-email",3,(char **)argv);
}

const char * JEApp::RandomQuote()
{
	const char *sillyQuotes[] = {
	"\"a f***ing weird video thingy\"",
	"\"you *must* listen to the voices\"",
	"\"Pyle!\"               \"Shazzam!\""
	};
	
	return (sillyQuotes[rand() % 3]);
}

////////
// main
////////
int main(int argc, char *argv[])
{
	JEApp app("application/x-vnd.prok-jeepers-elvis");
	
	// init random number generator
	srand(system_time());
	
	// run
	app.Run();
	
	return B_OK;
}