//CLVListItem header file
#ifndef CLV_LIST_ITEM_H
#define CLV_LIST_ITEM_H


//******************************************************************************************************
//**** PROJECT HEADER FILES AND CLASS NAME DECLARATIONS
//******************************************************************************************************
#include <ListItem.h>
#include <List.h>
#include <Region.h>

#include "ColumnListView.h"

class ColumnListView;


//******************************************************************************************************
//**** CLVItem CLASS DECLARATION
//******************************************************************************************************
class CLVListItem : public BListItem
{
	public:
		//Constructor and destructor
		CLVListItem(uint32 level = 0, bool superitem = false, bool expanded = false, float minheight = 0.0);
		virtual ~CLVListItem();

		//Archival stuff
		/* Not implemented yet
		CLVItem(BMessage* archive);
		static CLVItem* Instantiate(BMessage* data);
		virtual	status_t Archive(BMessage* data, bool deep = true) const;
		*/
		
		virtual void DrawItemColumn(BView* owner, BRect item_column_rect, int32 column_index,
			bool complete) = 0;	//column_index (0-N) is based on the order in which the columns were added
								//to the ColumnListView, not the display order.  An index of -1 indicates
								//that the program needs to draw a blank area beyond the last column.  The
								//main purpose is to allow the highlighting bar to continue all the way to
								//the end of the ColumnListView, even after the end of the last column.
		virtual void DrawItem(BView* owner, BRect itemRect, bool complete);
								//In general, you don't need or want to override DrawItem().

		virtual void Update(BView* owner, const BFont* font);

	private:
		friend class ColumnListView;

		
		float fMinHeight;
		BList* fSortingContextBList;
		ColumnListView* fSortingContextCLV;
};

#endif