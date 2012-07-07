#ifndef BFIV_VIEW_H
#define BFIV_VIEW_H

#ifndef	_VIEW_H
#include <View.h>
#endif
#ifndef	_REGION_H
#include <Region.h>
#endif
#ifndef	_BITMAP_H
#include <Bitmap.h>
#endif
#ifndef _OS_H
#include <OS.h>
#endif
#ifndef BFIV_TILER_H
#include "BFIV_Tiler.h"
#endif

class BFIV_View : public BView {
public:
									BFIV_View(const char *name, BFIV_Tiler *image);
									~BFIV_View();
virtual	void			Draw(BRect update);

void							SetImageScale(float scale);
float							ImageScale() const { return scaleFactor; };

static	int32			Draw_Thread_(void *data);
int32							Draw_Thread();


private:
BFIV_Tiler				*fatBastard;
BBitmap						*scratchMap;
BRegion						dirtyRegion;
sem_id						dirtySem;
thread_id					redrawThread;
float							scaleFactor;
};


#endif