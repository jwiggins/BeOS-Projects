/*
	MovieLooperUI.h
	MovieLooperUI class for Movie Looper (a Jeepers Elvis plugin)
	2000 John Wiggins
*/

#ifndef MOVIE_LOOPER_UI_H
#define MOVIE_LOOPER_UI_H

#include <interface/View.h>
#include <interface/ListView.h>
#include <interface/ScrollView.h>
#include <interface/RadioButton.h>
#include <interface/StringView.h>
#include <interface/Bitmap.h>
#include <interface/Rect.h>
#include <interface/Point.h>
#include <app/Message.h>
#include <support/String.h>
#include <posix/math.h>
#include <posix/stdio.h>

#include "VideoReader.h"
#include "MLMessages.h"

class MovieLooperUI : public BView {
public:
							MovieLooperUI(BRect frame, const char *name,
										VideoReader *vid, BBitmap **bmp,
										BLocker *lock);
							~MovieLooperUI();
	
	virtual	void			GetPreferredSize(float *w, float *h);
	virtual	void			AttachedToWindow();
	virtual	void			MessageReceived(BMessage *msg);

private:
	class ClipListView : public BListView {
	public:
							ClipListView(BRect frame, const char *name, BView *parent);
							~ClipListView();
		
		virtual	void		Draw(BRect update);
		virtual	void		MessageReceived(BMessage *msg);
		virtual	void		MouseUp(BPoint);
		virtual	void		MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
	private:
		rgb_color			highlightColor;
		bool				drawHighlight;
		BView				*parentView;
	};
	
	class PositionIndicator : public BView {
	public:
							PositionIndicator(BRect frame, const char *name);
		
		virtual	void		Draw(BRect update);
		virtual	void		MouseDown(BPoint pt);
		virtual	void		MouseUp(BPoint pt);
		virtual	void		MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
		virtual	void		KeyDown(const char *bytes, int32 numBytes);
		virtual	void		KeyUp(const char *bytes, int32 numBytes);
		virtual	void		MakeFocus(bool focusState = true);
		
		void				SetLength(int64 frames, bigtime_t dur);
		void				SetStart(int64 frame);
		void				SetStartWithRedraw(int64 frame);
		void				SetPosition(int64 frame);
		void				SetEnd(int64 frame);
		void				SetEndWithRedraw(int64 frame);
	private:
		void				StepStartForward();
		void				StepStartBackward();
		void				StepEndForward();
		void				StepEndBackward();
		void				ResetPoints();
		void				DrawKnob(float x);
		
		rgb_color			navigationColor;
		rgb_color			clipColor;
		rgb_color			unClipColor;
		rgb_color			knobColor;
		int64				frameCount;
		bigtime_t			duration;
		float				startCoord;
		float				playCoord;
		float				endCoord;
		int64				startPos;
		int64				playPos;
		int64				endPos;
		bool				startMoving;
		bool				endMoving;
	};
	
	PositionIndicator		*vidPosition;
	ClipListView			*listView;
	BRadioButton			*radio1, *radio2, *radio3;
	BStringView				*startStr, *endStr;
	VideoReader				*videoReader;
	BBitmap					**frameMap;
	BLocker					*frameMutex;
	float					prefWidth;
	float					prefHeight;
	clip_direction			curDirection;
};

#endif