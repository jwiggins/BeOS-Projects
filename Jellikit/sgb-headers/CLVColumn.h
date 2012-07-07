//Column list header header file
#ifndef CLV_COLUMN_H
#define CLV_COLUMN_H

//******************************************************************************************************
//**** PROJECT HEADER FILES AND CLASS NAME DECLARATIONS
//******************************************************************************************************
#include <SupportDefs.h>
#include <Window.h>
#include <string.h>

#include "ColumnListView.h"
#include "CLVListItem.h"
class ColumnListView;
class CLVColumn;
class CLVListItem;


//******************************************************************************************************
//**** CONSTANTS
//******************************************************************************************************
//Flags
enum
{
	CLV_SORT_KEYABLE =			0x00000001,		//Can be used as the sorting key
	CLV_NOT_MOVABLE =			0x00000002,		//Column can't be moved by user
	CLV_NOT_RESIZABLE =			0x00000004,		//Column can't be resized by user
	CLV_LOCK_AT_BEGINNING =		0x00000008,		//Movable columns may not be placed or moved by the user
												//into a position before this one
	CLV_LOCK_AT_END =			0x00000010,		//Movable columns may not be placed or moved by the user
												//into a position after this one
	CLV_HIDDEN =				0x00000020,		//This column is hidden initially
	CLV_MERGE_WITH_RIGHT =		0x00000040,		//Merge this column label with the one that follows it.
	CLV_LOCK_WITH_RIGHT =		0x00000080,		//Lock this column to the one that follows it such that
												//if the column to the right is moved by the user, this
												//one will move with it and vice versa
	CLV_EXPANDER =				0x00000100,		//Column contains an expander.  You may only use one
												//expander in a ColumnListView, and an expander may not be
												//added to a non-hierarchal ColumnListView.  It may not
												//have a label.  Its width is automatically set to 20.0.
												//The only flags that affect it are CLV_NOT_MOVABLE,
												//CLV_LOCK_AT_BEGINNING, CLV_NOT_SHOWN and
												//CLV_LOCK_WITH_RIGHT.  The others are set for you:
												//CLV_NOT_RESIZABLE | CLV_MERGE_WITH_RIGHT
	CLV_PUSH_PASS =				0x00000200		//Causes this column, if pushed by an expander to the
												//left, to pass that push on and also push the next
};												//column to the right.


//******************************************************************************************************
//**** ColumnListView CLASS DECLARATION
//******************************************************************************************************
class CLVColumn
{
	public:
		//Constructor and destructor
		CLVColumn(	const char* label,
					float width = 20.0,
					uint32 flags = 0,
					float min_width = 20.0);
		virtual ~CLVColumn();

		//Archival stuff
		/* Not implemented yet
		CLVColumn(BMessage* archive);
		static CLVColumn* Instantiate(BMessage* data);
		virtual	status_t Archive(BMessage* data, bool deep = true) const;
		*/

		//Functions
		float Width() const;
		virtual void SetWidth(float width);	//Can be overridden to detect changes to the column width
												//however since you are probably overriding
												//ColumnListView and dealing with an array of columns
												//anyway, it is probably more useful to override
												//ColumnListView::ColumnWidthChanged to detect changes to
												//column widths
		uint32 Flags() const;
		bool IsShown() const;
		void SetShown(bool shown);

	private:
		friend class ColumnListView;
		friend class CLVColumnLabelView;
		friend class CLVListItem;

		const char *fLabel;
		float fWidth;
		float fMinWidth;
		float fColumnBegin;
		float fColumnEnd;
		uint32 fFlags;
		bool fPushedByExpander;
		ColumnListView* fParent;
};

#endif