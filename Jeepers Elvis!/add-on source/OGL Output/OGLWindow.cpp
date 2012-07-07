/*
	OGLWindow.cpp
	2002 John Wiggins
*/

#include "OGLWindow.h"

OGLWindow::OGLWindow(BRect frame)
: BDirectGLWindow(frame, "OGL output", B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_CLOSABLE)
{
	tex_has_data = false;
	gl_initialized = false;
	quit_drawing = false;
	refresh_display = false;
	outputW = outputH = 0;
	texH = texW = 0;
	draw_thread = -1;
	currentDevice = -1;
	
	// find all the devices we can use
	EnumerateDevices( BGL_MONITOR_ANY, BGL_FASTEST | BGL_DOUBLE, BGL_ANY, BGL_NONE, BGL_NONE );
	// start GL on the last device (hardware)
	if( RestartGL(currentDevice)<B_OK)
	{
		// fall back to software if that fails
		if( RestartGL( BGL_DEVICE_SOFTWARE ) < B_OK )
			return; // blow up if software device fails
	}
	
	// start message loop, but stay offscreen
	Hide();
	Show();
}

OGLWindow::~OGLWindow()
{
	status_t ret;
	
	// leave the screen
	Hide();
	Sync();
	
	// tell our render thread to quit and then wait for it
	quit_drawing=true;
	wait_for_thread(draw_thread, &ret);
}

bool OGLWindow::QuitRequested()
{
	return true;
}

void OGLWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case OO_VISIBILITY_TOGGLE:
		{
			if (IsHidden())
			{
//				// make sure we're fullscreen
//				BRect scrFrm = BScreen(this).Frame();
//				ResizeTo(scrFrm.Width(), scrFrm.Height());
//			
//				be_app->HideCursor();
				Show();
			}
			else
			{
				Hide();
//				be_app->ShowCursor();
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

// DeviceInfo(...)
// called by EnumerateDevices() for each device found
void OGLWindow::DeviceInfo(uint32 device_id, uint32 monitor, const char *name, bool depth, bool stencil, bool accum)
{
	currentDevice++;
	devices[currentDevice].deviceID = device_id;
	devices[currentDevice].first = 0;
}

// VideoModeInfo(...)
// called by EnumerateDevices() for each mode supported by a particular device
void OGLWindow::VideoModeInfo(uint32 w, uint32 h, uint32 color, uint32 depth, uint32 stencil, uint32 accum)
{
	ModeInfo *m = new ModeInfo;
	m->next = 0;
	m->xres = w;
	m->yres = h;
	if( devices[currentDevice].first )
	{
		ModeInfo *last = devices[currentDevice].first;
		while( last->next )
			last = last->next;
		last->next = m;
	}
	else
	{
		devices[currentDevice].first = m;
	}
}

void OGLWindow::FrameResized(float w, float h)
{
	MakeCurrent();
		glViewport(0, 0, (int)w, (int)h);
	ReleaseCurrent();
}

// set the texture data
// XXX: this function _assumes_ that the bitmap data is B_RGB32

void OGLWindow::SetTextureData(BBitmap *bmp)
{	
	MakeCurrent();
		if(tex_has_data == false)
		{
//			printf("SetTextureData(): initial texture creation\n");
			// full load
			
			// cache the bitmap's dimensions
			outputW = bmp->Bounds().IntegerWidth() + 1;
			outputH = bmp->Bounds().IntegerHeight() + 1;
			// get an OpenGL friendly texture size
			determine_texture_size(outputW, outputH, &texW, &texH);
			
			// make the color component order correct
			fix_texture_color(bmp);
			
			// make some fake texture data
			uint32 *fakeTex;
			fakeTex = new uint32 [texW*texH];
			// blackness
			memset(fakeTex, 0, texW*texH*sizeof(uint32));

			// GL texture init
			glGenTextures(1, &tex_name);
			glBindTexture(GL_TEXTURE_2D, tex_name);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D, 0, 
							3, 
							texW,
							texH,
							0, GL_RGBA, GL_UNSIGNED_BYTE,
							fakeTex );
			glTexSubImage2D(GL_TEXTURE_2D, 0,
								0, 0,
								outputW,
								outputH,
								GL_RGBA, GL_UNSIGNED_BYTE,
								bmp->Bits());

			// clean up
			delete [] fakeTex;
			// set some flags
			tex_has_data = true;
			refresh_display = true;
		}
		else
		{
//			printf("SetTextureData(): texture data replacement\n");
			// make the color component order correct
			fix_texture_color(bmp);
			// replace the texture data
			glBindTexture(GL_TEXTURE_2D, tex_name);
			glTexSubImage2D(GL_TEXTURE_2D, 0,
								0, 0,
								outputW,
								outputH,
								GL_RGBA, GL_UNSIGNED_BYTE,
								bmp->Bits());
			refresh_display = true;
		}
	ReleaseCurrent();
}

status_t OGLWindow::RestartGL(uint32 device_id)
{
	status_t ret;
	
	//destroy old draw thread
	quit_drawing = true;
	if( draw_thread >= 0 )
		wait_for_thread( draw_thread, &ret );
	
	//make a new GL setup
	if(gl_initialized)
	{
		ShutdownGL();
		gl_initialized = false;
	}
	gl_initialized = true;
	quit_drawing = false;
	
	// initialize OpenGL
	if(InitializeGL( device_id, BGL_FASTEST | BGL_DOUBLE, BGL_ANY, BGL_NONE, BGL_NONE ) < B_OK)
	{
		printf( "OGLWindow::RestartGL -- Error initializing OpenGL\n" );
		fflush(stdout);
		be_app->PostMessage( B_QUIT_REQUESTED );
		return B_ERROR;
	}
	
	// Start the drawing thread.
	draw_thread = spawn_thread(render_thread, "OGL Output render", B_DISPLAY_PRIORITY, (void *)this);
	if( draw_thread < 0 )
	{
		fprintf( stderr, "Can't spawn drawing thread, bailing out...\n" );
		be_app->PostMessage(B_QUIT_REQUESTED);
		return B_ERROR;
	}

	// set the render thread in motion
	resume_thread( draw_thread );
	return B_OK;
}

// called after OpenGL is initialized
// set up projection matrix here, etc.
void OGLWindow::InitDisplayVars()
{
	//printf("InitDisplayVars()\n");
	MakeCurrent();
		// color stuff
		glClearColor(0.0,0.0,0.0,1.0); // black background
		glColor3f(1.0,1.0,1.0); // white drawing color
		
		// texture setup
		outputW = outputH = 0; // invalidate the output frame size
		texH = texW = 0; // and the texture's size
		tex_has_data = false; // no texture data yet
		glEnable(GL_TEXTURE_2D); // enable textures
		
		// viewing stuff
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
	ReleaseCurrent();
}

// voodoo3 only allows textures whose dimensions are powers of two.
// (width and height do not have to be equal)
// Given the size of an input frame, determine how big a texture
// should be to accomodate it (with room left over)
void OGLWindow::determine_texture_size(int32 inW, int32 inH, int32 *outW, int32 *outH)
{
	// zero the output sizes
	*outW = *outH = 0;
	
	// (1<<10) = 2048
	for (int i=1; i <= (1<<10); i<<=1)
	{
		// width
		if ( (*outW == 0) && (inW <= i) )
			*outW = i;
		// height
		if ( (*outH == 0) && (inH <= i) )
			*outH = i;
		
		// break when done
		if ( *outW && *outH )
			break;
	}
}

void OGLWindow::fix_texture_color(BBitmap *bmp)
{
	int32 width = bmp->Bounds().IntegerWidth()+1,
			height = bmp->Bounds().IntegerHeight()+1;
	uint32 *bits = (uint32 *)bmp->Bits();
	int32 rowlen = bmp->BytesPerRow() / 4;
	uint32 val;
	uint32 red, green, blue;
	
	for (int32 y = 0; y <= height; y++)
	{
		for (int32 x = 0; x <= width; x++)
		{
			val = bits[(y*rowlen)+x];
			
			// pixel components
			red = (val>>16 & 0xff);
			green = (val>>8 & 0xff);
			blue = (val & 0xff);
			
			// smoosh back together
			// B_RGB32 -> GL_RGBA : 1234 -> 1432
			//val = (uint32)( ((red<<16) | (green<<8) | blue) & 0x00ffffff);
			val = (uint32)( ((blue<<16) | (green<<8) | red) & 0x00ffffff);
			
			// assign
			bits[(y*rowlen)+x] = val;
		}
	}
	
}

int32 OGLWindow::render_thread(void *data)
{
	OGLWindow *win = (OGLWindow *)data;
	
	// setup the display!
	win->InitDisplayVars();
	
	win->MakeCurrent();
		while(win->quit_drawing == false)
		{
			// only if we have a reason to draw
			if (win->refresh_display)
			{
				// OpenGL calls go here
				glClear(GL_COLOR_BUFFER_BIT);
				if (win->tex_has_data)
				{
					float texX, texY;
					
					texX = (float)win->outputW / (float)win->texW;
					texY = (float)win->outputH / (float)win->texH;
					//printf("render_thread(): rendering with texture\n");
					glBindTexture(GL_TEXTURE_2D, win->tex_name);
					//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
					glBegin(GL_QUADS);
						glTexCoord2f(0.0, 0.0); glVertex3f(-1.0,  1.0,  0.0);
						glTexCoord2f(texX, 0.0); glVertex3f( 1.0,  1.0,  0.0);
						glTexCoord2f(texX, texY); glVertex3f( 1.0, -1.0,  0.0);
						glTexCoord2f(0.0, texY); glVertex3f(-1.0, -1.0,  0.0);
					glEnd();
				}
				else
				{
					//printf("render_thread(): rendering without texture\n");
					glBegin(GL_QUADS);
						glVertex3f(-1.0, -1.0,  1.0);
						glVertex3f( 1.0, -1.0,  1.0);
						glVertex3f( 1.0,  1.0,  1.0);
						glVertex3f(-1.0,  1.0,  1.0);
					glEnd();
				}
				//glFlush();
				
				// draw to screen (double buffered)
				win->SwapBuffers();
				glDrawBuffer(GL_BACK);
				
				// unset the refresh flag
				win->refresh_display = false;
			}
						
			win->YieldCurrent();
			snooze(10 * 1000); // check for refresh opportunity every 10 ms
		}
	win->ReleaseCurrent();
}

