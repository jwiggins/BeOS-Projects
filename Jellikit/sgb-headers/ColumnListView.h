//Column list view header file
#ifndef COLUMN_LIST_VIEW_H
#define COLUMN_LIST_VIEW_H


//******************************************************************************************************
//**** PROJECT HEADER FILES AND CLASS NAME DECLARATIONS
//******************************************************************************************************
#include <ListView.h>
#include <Rect.h>
#include <Point.h>
#include <Font.h>
#include <Bitmap.h>
#include <ScrollView.h>
#include <List.h>
#include <AppFileInfo.h>
#include <File.h>
#include <math.h>

#include "Colors.h"
#include "CLVColumn.h"
#include "CLVListItem.h"

class CLVColumn;
class CLVListItem;
class CLVColumnLabelView;
class CLVContainerView;

//******************************************************************************************************
//**** ColumnListView CLASS DECLARATION
//******************************************************************************************************
class ColumnListView : public BListView
{
	public:
		//Constructor and destructor
		ColumnListView(	BRect Frame,
						BScrollView** ContainerView,	//Used to get back a pointer to the container
														//view that will hold the ColumnListView, the
														//the CLVColumnLabelView, and the scrollbars.
														//If no scroll bars or border are asked for,
														//this will act like a plain BView container.
						const char* Name = NULL,
						uint32 ResizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
						uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE,
						list_view_type Type = B_SINGLE_SELECTION_LIST,
						bool hierarchical = false,
						bool horizontal = true,					//Which scroll bars should I add, if any
						bool vertical = true,
						border_style border = B_NO_BORDER,		//What type of border to add, if any
						const BFont* LabelFont = be_plain_font);
		virtual ~ColumnListView();

		//Archival stuff
			/*** Not implemented yet
		ColumnListView(BMessage* archive);
		static ColumnListView* Instantiate(BMessage* data);
		virtual	status_t Archive(BMessage* data, bool deep = true) const;
			***/

		//Column setup functions
		virtual bool AddColumn(CLVColumn* Column);			//Note that a column may only be added to
															//one ColumnListView at a time, and may not
															//be added more than once to the same
															//ColumnListView without removing it
															//inbetween
		virtual bool AddColumnList(BList* NewColumns);
		virtual bool RemoveColumn(CLVColumn* Column);
		virtual bool RemoveColumns(CLVColumn* Column, int32 Count);	//Finds Column in ColumnList
																	//and removes Count columns and
																	//their data from the view
																	//and its items
		int32 CountColumns() const;
		int32 IndexOfColumn(CLVColumn* column) const;
		CLVColumn* ColumnAt(int32 column_index) const;
		virtual bool SetDisplayOrder(const int32* Order);
			//Sets the display order: each int32 in the Order list specifies the column index of the
			//next column to display.  Note that this DOES NOT get called if the user drags a
			//column, so overriding it will not inform you of user changes.  If you need that info,
			//override DisplayOrderChanged instead.  Also note that SetDisplayOrder does call
			//DisplayOrderChanged(false).
		int32* DisplayOrder() const;	//Gets the display order in the same format as that used by
										//SetDisplayOrder.  The returned array belongs to the caller and
										//must be delete[]'d when done with it.
		//BView overrides
		virtual void FrameResized(float Width, float Height);
		virtual void AttachedToWindow();
 		virtual void ScrollTo(BPoint point);
 		virtual void SelectionChanged();
 		virtual	void MouseDown(BPoint point);
 		virtual void MouseUp(BPoint point);
 		virtual void MouseMoved(BPoint point, uint32 code, const BMessage *msg);
 		virtual	bool InitiateDrag(BPoint point, int32 index, bool wasSelected);
 
		//List functions
		virtual bool AddList(BList* newItems);						//This must be a BList of
																	//CLVListItem*'s, NOT BListItem*'s
		virtual bool AddList(BList* newItems, int32 fullListIndex);	//This must be a BList of
																	//CLVListItem*'s, NOT BListItem*'s
		virtual bool RemoveItem(CLVListItem* item);
		virtual void MakeEmpty();
		CLVListItem* FullListItemAt(int32 fullListIndex)  const;
		int32 FullListIndexOf(const CLVListItem* item) const;
		int32 FullListIndexOf(BPoint point) const;
		CLVListItem* FullListFirstItem() const;
		CLVListItem* FullListLastItem() const;
		bool FullListHasItem(const CLVListItem* item) const;
		int32 FullListCountItems() const;
		bool FullListIsEmpty() const;
		int32 FullListCurrentSelection(int32 index = 0) const;
		
	

	private:
		friend class CLVMainView;
		friend class CLVColumn;
		friend class CLVColumnLabelView;
		friend class CLVListItem;

		void UpdateColumnSizesDataRectSizeScrollBars();
		void UpdateScrollBars();
		void ColumnsChanged();
		void CreateContainer(bool horizontal, bool vertical, border_style border, uint32 ResizingMode,
			uint32 flags);
		void MakeEmptyPrivate();
		bool AddListPrivate(BList* newItems, int32 fullListIndex);

		CLVColumnLabelView* fColumnLabelView;
		CLVContainerView* fScrollView;
		BList fColumnList;
		BList fColumnDisplayList;
		float fDataWidth,fDataHeight,fPageWidth,fPageHeight;
		BList fSortKeyList;		//List contains CLVColumn pointers
		BList fFullItemList;
		
		// my stuff
		BBitmap *icon_bitmap;
		bool mouse_is_down, currently_dragging;
		BPoint mouse_down_point;
};
#endif