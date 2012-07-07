/*

	AboutWindow.cpp - A standard about window. (with easter egg)
	John Wiggins 1998-1999

*/

#include "AboutWindow.h"

JellikitAboutWindow::JellikitAboutWindow(BRect frame, bool have_media)
: BWindow(frame, "", B_MODAL_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_ZOOMABLE|B_NOT_RESIZABLE), media_server_is_alive(have_media)
{
	BResources *resources;
	const void *sound_rez = NULL;
	
	resources = be_app->AppResources(); // mmmm Easy API
	sound_rez = resources->LoadResource(B_RAW_TYPE, "jellikit:sound", &rez_size); // grab a pointer to the sound
	
	if(sound_rez != NULL)
	{
		sound_ptr = (char *)malloc(rez_size); // make space for a copy of the sound
		memcpy((void *)sound_ptr, sound_rez, rez_size); // copy the sound
	}
	else
		sound_ptr = NULL; // make it NULL if we couldn't grab the sound. That way we don't do anything silly w/ it.
	
	resources = NULL; // stop pointing
	
	// install an AboutView
	AddChild(new AboutView(Bounds()));
	
	// set the pulse rate for the about view
	SetPulseRate(100 * 1000);
	
	// show!
	Show();
	//printf("JellikitAboutWindow: we are Show()ing!\n");
}

bool JellikitAboutWindow::QuitRequested()
{
	//printf("AboutWindow::QuitRequested\n");
	free(sound_ptr);
	be_app->PostMessage(ABOUT_BOX_DYING);
	return true;
}

void JellikitAboutWindow::MessageReceived(BMessage *msg)
{
	//printf("JellikitAboutWindow::MessageReceived\n");
	switch(msg->what)
	{
		case DO_EASTER_EGG:
		{			
			//printf("JellikitAboutWindow: got an easter egg message.\n");
			// jellikit.raw is 16bit stereo lendian
			
			// fail if we don't have the sound or media services
			if((sound_ptr == NULL) || !media_server_is_alive) 
				;
			else
			{
				//printf("JellikitAboutWindow: sound_ptr is valid.\n");
				//printf("JellikitAboutWindow: media_server is alive.\n");
				
				if(find_thread("jellikit:easteregg") < 0)
				{
					resume_thread(spawn_thread(EasterEgg, "jellikit:easteregg", B_NORMAL_PRIORITY, (void *)this));
				}
			}
			
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}

int32 JellikitAboutWindow::EasterEgg(void *data)
{
	((JellikitAboutWindow *)data)->PlayEESound();
	return B_OK;
}

int32 JellikitAboutWindow::PlayEESound()
{
	//printf("JellikitAboutWindow: playing easter egg sound.\n");
	status_t err = B_ERROR; // we need to check the return val of some calls
	BSoundPlayer player; // make a vanilla soundplayer
	BSoundPlayer::play_id id; // make an id for later
	media_raw_audio_format format; // make a format to fill w/ info
	
	//printf("JellikitAboutWindow: BSoundPlayer constructed.\n");
	format.frame_rate = 44100.; // 44.1 khz sample rate
	format.channel_count = 2; // stereo
	format.format = media_raw_audio_format::B_AUDIO_SHORT; // 16 bit
	format.byte_order = B_MEDIA_LITTLE_ENDIAN; // little endian
	format.buffer_size = 4096; // This was a guess, but it sounds good
	
	//printf("JellikitAboutWindow: starting the soundplayer...\n");
	err = player.Start(); // start the player
	if(err == B_NO_ERROR)
	{
		//printf("JellikitAboutWindow: adjusting volume.\n");
		player.SetVolume(1.); // jack the volume up
		//printf("JellikitAboutWindow: playing sound.\n");
		id = player.StartPlaying(new BSound((void *)sound_ptr, rez_size, format)); // play the sound
		player.WaitForSound(id); // wait for it to finish
	}
	return B_OK;
}