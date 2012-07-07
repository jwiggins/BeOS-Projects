/*
	OGLWindow.h
	2002 John Wiggins
	
	Most of this code was stolen from the teapot example
	provided with Be's OGL beta.
*/

#ifndef OGL_WINDOW_H
#define OGL_WINDOW_H

#include <app/Application.h>
#include <app/Message.h>
#include <interface/Rect.h>
#include <interface/Bitmap.h>
#include <interface/Screen.h>
#include <kernel/OS.h>
#include <opengl/DirectGLWindow.h>
#include <opengl/GL/glu.h>
#include <posix/string.h>
#include <posix/stdio.h>

enum {
	OO_VISIBILITY_TOGGLE = 'oovt',
};

#define MAX_DEVICES 16
typedef struct ModeInfoRec ModeInfo;
struct ModeInfoRec
{
	ModeInfo *next;
	uint32 xres;
	uint32 yres;
};

typedef struct GfxDeviceRec
{
	uint32 deviceID;
	ModeInfo *first;
} GfxDevice;

class OGLWindow : public BDirectGLWindow {
public:
						OGLWindow(BRect frame);
	virtual				~OGLWindow();
	
	virtual	bool		QuitRequested();
	virtual	void		MessageReceived(BMessage *msg);
	virtual void		DeviceInfo(uint32 device_id, uint32 monitor, const char *name, bool depth, bool stencil, bool accum);
	virtual void		VideoModeInfo(uint32 w, uint32 h, uint32 color, uint32 depth, uint32 stencil, uint32 accum);
	virtual void		FrameResized(float w, float h);
	
	void				InitDisplayVars();
	void				SetTextureData(BBitmap *bmp);
	
	GLuint				tex_name;
	int32				outputW, outputH;
	int32				texH, texW;
	bool				quit_drawing;
	bool				tex_has_data;
	bool				refresh_display;
	
private:
	
	status_t			RestartGL(uint32 device_id);
	void				determine_texture_size(int32 iW, int32 iH, int32 *oW, int32 *oH);
	void				fix_texture_color(BBitmap *bmp);
	static int32		render_thread(void *data);
	
	thread_id			draw_thread;
	int32				currentDevice;
	GfxDevice			devices[MAX_DEVICES];
	bool				gl_initialized;
	
};

#endif