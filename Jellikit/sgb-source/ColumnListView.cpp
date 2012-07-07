//Column list view source file

//******************************************************************************************************
//**** PROJECT HEADER FILES
//******************************************************************************************************
#define ColumnListView_CPP
#include "ColumnListView.h"
#include "CLVColumnLabelView.h"
#include "CLVColumn.h"
#include "CLVListItem.h"

// needed for SelectionChanged()
#include "AttrWindow.h"
#include "AttrItem.h"

//******************************************************************************************************
//**** ColumnListView CLASS DEFINITION
//******************************************************************************************************
class CLVContainerView : public BScrollView
{
	public:
		CLVContainerView(char* name, BView* target, uint32 resizingMode, uint32 flags, bool horizontal,
			bool vertical, border_style border);
		~CLVContainerView();
		bool IsBeingDestroyed;
};


CLVContainerView::CLVContainerView(char* name, BView* target, uint32 resizingMode, uint32 flags,
	bool horizontal, bool vertical, border_style border) :
BScrollView(name,target,resizingMode,flags,horizontal,vertical,border)
{
	IsBeingDestroyed = false;
};


CLVContainerView::~CLVContainerView()
{
	IsBeingDestroyed = true;
}


ColumnListView::ColumnListView(BRect Frame, BScrollView **ContainerView, const char *Name,
	uint32 ResizingMode, uint32 flags, list_view_type Type, bool _UNUSED(hierarchical), bool horizontal,
	bool vertical, border_style border, const BFont *LabelFont)
: BListView(Frame,Name,Type,B_FOLLOW_ALL_SIDES,flags),
fColumnList(6),
fColumnDisplayList(6),
fSortKeyList(6),
fFullItemList(32)
{

	//Create the column titles bar view
	font_height FontAttributes;
	LabelFont->GetHeight(&FontAttributes);
	float fLabelFontHeight = ceil(FontAttributes.ascent) + ceil(FontAttributes.descent);
	float ColumnLabelViewBottom = Frame.top+1.0+fLabelFontHeight+3.0;
	fColumnLabelView = new CLVColumnLabelView(BRect(Frame.left,Frame.top,Frame.right,
		ColumnLabelViewBottom),this,LabelFont);

	//Create the container view
	CreateContainer(horizontal,vertical,border,ResizingMode,flags);
	*ContainerView = fScrollView;

	//Complete the setup
	UpdateColumnSizesDataRectSizeScrollBars();
	fColumnLabelView->UpdateDragGroups();
	
	// grab the app icon
	app_info ai; 
	BFile file;
	BAppFileInfo file_info;
	
	icon_bitmap = new BBitmap(BRect(0,0,15,15), B_COLOR_8_BIT);
	be_app->GetAppInfo(&ai);
	file.SetTo(&ai.ref, B_READ_WRITE);
	file_info.SetTo(&file);
	file_info.GetIcon(icon_bitmap, B_MINI_ICON);
}


ColumnListView::~ColumnListView()
{
	//Delete all list columns
	int32 ColumnCount = fColumnList.CountItems();
	for(int32 Counter = ColumnCount-1; Counter >= 0; Counter--)
	{
		CLVColumn* Item = (CLVColumn*)fColumnList.RemoveItem(Counter);
		if(Item)
			delete Item;
	}
	//Remove and delete the container view if necessary
	if(!fScrollView->IsBeingDestroyed)
	{
		fScrollView->RemoveChild(this);
		delete fScrollView;
	}
	// free the app icon bitmap
	delete icon_bitmap;
}


void ColumnListView::CreateContainer(bool horizontal, bool vertical, border_style border,
	uint32 ResizingMode, uint32 flags)
{
	BRect ViewFrame = Frame();
	BRect LabelsFrame = fColumnLabelView->Frame();

	fScrollView = new CLVContainerView("Attribute List Container",this,ResizingMode,flags,horizontal,vertical,border);
	BRect NewFrame = Frame();
	//Resize the main view to make room for the CLVColumnLabelView
	ResizeTo(ViewFrame.right-ViewFrame.left,ViewFrame.bottom-LabelsFrame.bottom-1.0);
	MoveTo(NewFrame.left,NewFrame.top+(LabelsFrame.bottom-LabelsFrame.top+1.0));
	fColumnLabelView->MoveTo(NewFrame.left,NewFrame.top);

	//Add the ColumnLabelView
	fScrollView->AddChild(fColumnLabelView);

	//Remove and re-add the BListView so that it will draw after the CLVColumnLabelView
	fScrollView->RemoveChild(this);
	fScrollView->AddChild(this);
}

void ColumnListView::UpdateColumnSizesDataRectSizeScrollBars()
{
	//Figure out the width
	float ColumnBegin;
	float ColumnEnd = -1.0;
	fDataWidth = 0.0;
	//bool NextPushedByExpander = false;
	int32 NumberOfColumns = fColumnDisplayList.CountItems();
	for(int32 Counter = 0; Counter < NumberOfColumns; Counter++)
	{
		CLVColumn* Column = (CLVColumn*)fColumnDisplayList.ItemAt(Counter);
		if(Column->IsShown())
		{
			float ColumnWidth = Column->Width();
			ColumnBegin = ColumnEnd + 1.0;
			ColumnEnd = ColumnBegin + ColumnWidth;
			Column->fColumnBegin = ColumnBegin;
			Column->fColumnEnd = ColumnEnd;
			fDataWidth = Column->fColumnEnd;
		}
	}

	//Figure out the height
	fDataHeight = 0.0;
	int32 NumberOfItems = CountItems();
	for(int32 Counter2 = 0; Counter2 < NumberOfItems; Counter2++)
		fDataHeight += ItemAt(Counter2)->Height()+1.0;
	if(NumberOfItems > 0)
		fDataHeight -= 1.0;

	//Update the scroll bars
	UpdateScrollBars();
}


void ColumnListView::UpdateScrollBars()
{
	if(fScrollView)
	{
		//Figure out the bounds and scroll if necessary
		BRect ViewBounds;
		float DeltaX,DeltaY;
		do
		{
			ViewBounds = Bounds();
			//Figure out the width of the page rectangle
			fPageWidth = fDataWidth;
			fPageHeight = fDataHeight;
			//If view runs past the end, make more visible at the beginning
			DeltaX = 0.0;
			if(ViewBounds.right > fDataWidth && ViewBounds.left > 0)
			{
				DeltaX = ViewBounds.right-fDataWidth;
				if(DeltaX > ViewBounds.left)
					DeltaX = ViewBounds.left;
			}
			DeltaY = 0.0;
			if(ViewBounds.bottom > fDataHeight && ViewBounds.top > 0)
			{
				DeltaY = ViewBounds.bottom-fDataHeight;
				if(DeltaY > ViewBounds.top)
					DeltaY = ViewBounds.top;
			}
			if(DeltaX != 0.0 || DeltaY != 0.0)
			{
				ScrollTo(BPoint(ViewBounds.left-DeltaX,ViewBounds.top-DeltaY));
				ViewBounds = Bounds();
			}
			if(ViewBounds.right-ViewBounds.left > fDataWidth)
				fPageWidth = ViewBounds.right;
			if(ViewBounds.bottom-ViewBounds.top > fDataHeight)
				fPageHeight = ViewBounds.bottom;
		}while(DeltaX != 0.0 || DeltaY != 0.0);
	
		//Figure out the ratio of the bounds rectangle width or height to the page rectangle width or height
		float WidthProp = (ViewBounds.right-ViewBounds.left)/fPageWidth;
		float HeightProp = (ViewBounds.bottom-ViewBounds.top)/fPageHeight;

		BScrollBar* HScrollBar = fScrollView->ScrollBar(B_HORIZONTAL);
		BScrollBar* VScrollBar = fScrollView->ScrollBar(B_VERTICAL);
		//Set the scroll bar ranges and proportions.  If the whole document is visible, inactivate the
		//slider
		if(HScrollBar)
		{
			if(WidthProp >= 1.0 && ViewBounds.left == 0.0)
				HScrollBar->SetRange(0.0,0.0);
			else
				HScrollBar->SetRange(0.0,fPageWidth-(ViewBounds.right-ViewBounds.left));
			HScrollBar->SetProportion(WidthProp);
			//Set the step values
			HScrollBar->SetSteps(20.0,ViewBounds.right-ViewBounds.left);
		}
		if(VScrollBar)
		{
			if(HeightProp >= 1.0 && ViewBounds.top == 0.0)
			{
				VScrollBar->SetRange(0.0,0.0);
			}
			else
			{
				VScrollBar->SetRange(0.0,fPageHeight-(ViewBounds.bottom-ViewBounds.top));
			}
			VScrollBar->SetProportion(HeightProp);
		}
	}
}


void ColumnListView::ColumnsChanged()
{
	//Any previous column dragging/resizing will get corrupted, so deselect
	if(fColumnLabelView->fColumnClicked)
		fColumnLabelView->fColumnClicked = NULL;

	//Update the internal sizes and grouping of the columns and sizes of drag groups
	UpdateColumnSizesDataRectSizeScrollBars();
	fColumnLabelView->UpdateDragGroups();
	fColumnLabelView->Invalidate();
	Invalidate();
}


bool ColumnListView::AddColumn(CLVColumn* Column)
//Adds a column to the ColumnListView at the end of the list.  Returns true if successful.
{
	int32 NumberOfColumns = fColumnList.CountItems();
	int32 DisplayIndex = NumberOfColumns;

	//Make sure this column hasn't already been added to another ColumnListView
	if(Column->fParent != NULL)
		return false;

	BWindow* ParentWindow = Window();
	if(ParentWindow)
		ParentWindow->Lock();
	//Check if this should be locked at the beginning or end, and adjust its position if necessary
	if(!Column->Flags() & CLV_LOCK_AT_END)
	{
		bool Repeat;
		if(Column->Flags() & CLV_LOCK_AT_BEGINNING)
		{
			//Move it to the beginning, after the last CLV_LOCK_AT_BEGINNING item
			DisplayIndex = 0;
			Repeat = true;
			while(Repeat && DisplayIndex < NumberOfColumns)
			{
				Repeat = false;
				CLVColumn* LastColumn = (CLVColumn*)fColumnDisplayList.ItemAt(DisplayIndex);
				if(LastColumn->Flags() & CLV_LOCK_AT_BEGINNING)
				{
					DisplayIndex++;
					Repeat = true;
				}
			}
		}
		else
		{
			//Make sure it isn't after a CLV_LOCK_AT_END item
			Repeat = true;
			while(Repeat && DisplayIndex > 0)
			{
				Repeat = false;
				CLVColumn* LastColumn = (CLVColumn*)fColumnDisplayList.ItemAt(DisplayIndex-1);
				if(LastColumn->Flags() & CLV_LOCK_AT_END)
				{
					DisplayIndex--;
					Repeat = true;
				}
			}
		}
	}

	//Add the column to the display list in the appropriate position
	fColumnDisplayList.AddItem(Column, DisplayIndex);

	//Add the column to the end of the column list
	fColumnList.AddItem(Column);

	//Tell the column it belongs to me now
	Column->fParent = this;

	//Set the scroll bars and tell views to update
	ColumnsChanged();
	if(ParentWindow)
		ParentWindow->Unlock();
	return true;
}


bool ColumnListView::AddColumnList(BList* NewColumns)
//Adds a BList of CLVColumn's to the ColumnListView at the position specified, or at the end of the list
//if AtIndex == -1.  Returns true if successful.
{
	int32 NumberOfColumns = int32(fColumnList.CountItems());
	int32 NumberOfColumnsToAdd = int32(NewColumns->CountItems());
	int32 Counter;
	
	//Make sure none of these columns have already been added to a ColumnListView
	for(Counter = 0; Counter < NumberOfColumnsToAdd; Counter++)
		if(((CLVColumn*)NewColumns->ItemAt(Counter))->fParent != NULL)
			return false;
	//Make sure none of these columns are being added twice
	for(Counter = 0; Counter < NumberOfColumnsToAdd-1; Counter++)
		for(int32 Counter2 = Counter+1; Counter2 < NumberOfColumnsToAdd; Counter2++)
			if(NewColumns->ItemAt(Counter) == NewColumns->ItemAt(Counter2))
				return false;

	BWindow* ParentWindow = Window();
	if(ParentWindow)
		ParentWindow->Lock();
	for(Counter = 0; Counter < NumberOfColumnsToAdd; Counter++)
	{
		CLVColumn* Column = (CLVColumn*)NewColumns->ItemAt(Counter);
		//Check if this should be locked at the beginning or end, and adjust its position if necessary
		int32 DisplayIndex = NumberOfColumns;
		if(!Column->Flags() & CLV_LOCK_AT_END)
		{
			bool Repeat;
			if(Column->Flags() & CLV_LOCK_AT_BEGINNING)
			{
				//Move it to the beginning, after the last CLV_LOCK_AT_BEGINNING item
				DisplayIndex = 0;
				Repeat = true;
				while(Repeat && DisplayIndex < NumberOfColumns)
				{
					Repeat = false;
					CLVColumn* LastColumn = (CLVColumn*)fColumnDisplayList.ItemAt(DisplayIndex);
					if(LastColumn->Flags() & CLV_LOCK_AT_BEGINNING)
					{
						DisplayIndex++;
						Repeat = true;
					}
				}
			}
			else
			{
				//Make sure it isn't after a CLV_LOCK_AT_END item
				Repeat = true;
				while(Repeat && DisplayIndex > 0)
				{
					Repeat = false;
					CLVColumn* LastColumn = (CLVColumn*)fColumnDisplayList.ItemAt(DisplayIndex-1);
					if(LastColumn->Flags() & CLV_LOCK_AT_END)
					{
						DisplayIndex--;
						Repeat = true;
					}
				}
			}
		}

		//Add the column to the display list in the appropriate position
		fColumnDisplayList.AddItem(Column, DisplayIndex);

		//Tell the column it belongs to me now
		Column->fParent = this;

		NumberOfColumns++;
	}

	//Add the columns to the end of the column list
	fColumnList.AddList(NewColumns);

	//Set the scroll bars and tell views to update
	ColumnsChanged();
	if(ParentWindow)
		ParentWindow->Unlock();
	return true;
}


bool ColumnListView::RemoveColumn(CLVColumn* Column)
//Removes a CLVColumn from the ColumnListView.  Returns true if successful.
{
	if(!fColumnList.HasItem(Column))
		return false;
	int32 ColumnIndex = fSortKeyList.IndexOf(Column);
	if(ColumnIndex >= 0)
		fSortKeyList.RemoveItem(ColumnIndex);

	BWindow* ParentWindow = Window();
	if(ParentWindow)
		ParentWindow->Lock();
	//Remove Column from the column and display lists
	fColumnDisplayList.RemoveItem(Column);
	fColumnList.RemoveItem(Column);

	//Tell the column it has been removed
	Column->fParent = NULL;

	//Set the scroll bars and tell views to update
	ColumnsChanged();
	if(ParentWindow)
		ParentWindow->Unlock();
	return true;
}


bool ColumnListView::RemoveColumns(CLVColumn* Column, int32 Count)
//Finds Column in ColumnList and removes Count columns and their data from the view and its items
{
	BWindow* ParentWindow = Window();
	if(ParentWindow)
		ParentWindow->Lock();
	int32 ColumnIndex = fColumnList.IndexOf(Column);
	if(ColumnIndex < 0)
	{
		if(ParentWindow)
			ParentWindow->Unlock();
		return false;
	}
	if(ColumnIndex + Count >= fColumnList.CountItems())
	{
		if(ParentWindow)
			ParentWindow->Unlock();
		return false;
	}

	//Remove columns from the column and display lists
	for(int32 Counter = ColumnIndex; Counter < ColumnIndex+Count; Counter++)
	{
		CLVColumn* ThisColumn = (CLVColumn*)fColumnList.ItemAt(Counter);
		fColumnDisplayList.RemoveItem(ThisColumn);

		int32 SortIndex = fSortKeyList.IndexOf(Column);
		if(SortIndex >= 0)
			fSortKeyList.RemoveItem(SortIndex);

		//Tell the column it has been removed
		ThisColumn->fParent = NULL;
	}
	fColumnList.RemoveItems(ColumnIndex,Count);

	//Set the scroll bars and tell views to update
	ColumnsChanged();
	if(ParentWindow)
		ParentWindow->Unlock();
	return true;
}


int32 ColumnListView::CountColumns() const
{
	return fColumnList.CountItems();
}


int32 ColumnListView::IndexOfColumn(CLVColumn* column) const
{
	return fColumnList.IndexOf(column);
}


CLVColumn* ColumnListView::ColumnAt(int32 column_index) const
{
	return (CLVColumn*)fColumnList.ItemAt(column_index);
}


bool ColumnListView::SetDisplayOrder(const int32* ColumnOrder)
//Sets the display order using a BList of CLVColumn's
{
	BWindow* ParentWindow = Window();
	if(ParentWindow)
		ParentWindow->Lock();
	//Add the items to the display list in order
	fColumnDisplayList.MakeEmpty();
	int32 ColumnsToSet = fColumnList.CountItems();
	for(int32 Counter = 0; Counter < ColumnsToSet; Counter++)
	{
		if(ColumnOrder[Counter] >= ColumnsToSet)
		{
			if(ParentWindow)
				ParentWindow->Unlock();
			return false;
		}
		for(int32 Counter2 = 0; Counter2 < Counter; Counter2++)
			if(ColumnOrder[Counter] == ColumnOrder[Counter2])
			{
				if(ParentWindow)
					ParentWindow->Unlock();
				return false;
			}
		fColumnDisplayList.AddItem(fColumnList.ItemAt(ColumnOrder[Counter]));
	}

	//Update everything about the columns
	ColumnsChanged();

	//Let the program know that the display order changed.
	if(ParentWindow)
		ParentWindow->Unlock();
	return true;
}


int32* ColumnListView::DisplayOrder() const
{
	int32 ColumnsInList = fColumnList.CountItems();
	int32* ReturnList = new int32[ColumnsInList];
	BWindow* ParentWindow = Window();
	if(ParentWindow)
		ParentWindow->Lock();
	for(int32 Counter = 0; Counter < ColumnsInList; Counter++)
		ReturnList[Counter] = int32(fColumnList.IndexOf(fColumnDisplayList.ItemAt(Counter)));
	if(ParentWindow)
		ParentWindow->Unlock();
	return ReturnList;
}

void ColumnListView::FrameResized(float width, float height)
{
	UpdateColumnSizesDataRectSizeScrollBars();
	int32 NumberOfItems = CountItems();
	BFont Font;
	GetFont(&Font);
	for(int32 Counter = 0; Counter < NumberOfItems; Counter++)
		ItemAt(Counter)->Update(this,&Font);
	BListView::FrameResized(width,height);
}


void ColumnListView::AttachedToWindow()
//Hack to work around app_server bug
{
	BListView::AttachedToWindow();
	UpdateColumnSizesDataRectSizeScrollBars();
}


void ColumnListView::ScrollTo(BPoint point)
{
	BListView::ScrollTo(point);
	fColumnLabelView->ScrollTo(BPoint(point.x,0.0));
}


void ColumnListView::SelectionChanged()
{
	BListView::SelectionChanged();
	//printf("ColumnListView::SelectionChanged()\n");
	// toss a view into the editor window for the selected attribute
	AttrItem *selected = static_cast<AttrItem *>(ItemAt(CurrentSelection()));
	AttrWindow *window = static_cast<AttrWindow *>(Window());
	
	if(selected != NULL)
	{
		if(selected->HasAddon())
		{
			// its ok to proceed
			window->EditItem(selected->AttrName(), selected->Id());
			//printf("ColumnListView::SelectionChanged(). message sent.\n");
		}
		else
		{
			window->EditItem(NULL,-1);
		}
	}
	else
	{
		window->EditItem(NULL,-1);
	}
	selected = NULL;
	window = NULL;
}

void ColumnListView::MouseDown(BPoint point)
{
	BListView::MouseDown(point);
	//printf("ColumnListView::MouseDown()\n");
	
	if(!Window()->IsActive())
		Window()->Activate();
	
	SetMouseEventMask(B_POINTER_EVENTS);
	mouse_is_down = true;
}

void ColumnListView::MouseUp(BPoint _UNUSED(point))
{
	//printf("ColumnListView::MouseUp()\n");
	mouse_is_down = false;
	currently_dragging = false;
}

//#include "dbg_head.h"
//#include "drag_icon.h"
void ColumnListView::MouseMoved(BPoint point, uint32 _UNUSED(code), const BMessage *_UNUSED(msg))
{
	//printf("ColumnListView::MouseMoved()\n");
	if(!mouse_is_down || currently_dragging)
		return; // bail if the mouse isn't down, or we are already dragging
	
	// check the distance from the mouse_down_point to point
	// greater than 5 and we start dragging
	float dx=0,dy=0,dz=0;
	dx = mouse_down_point.x - point.x;
	dy = mouse_down_point.y - point.y;
	dz = sqrt(dx*dx + dy*dy);
	if(dz < 5.0)
		return;
	
	BMessage drag_msg(DRAGGED_ATTRIBUTE);
	BBitmap *drag_bitmap;
	BPoint drag_point;
	BView *a_view;
	const char *name_ptr;
	float name_width=0.;
	
	AttrItem *selected = static_cast<AttrItem *>(ItemAt(CurrentSelection()));
	AttrWindow *window = static_cast<AttrWindow *>(Window());
	
	//memcpy(icon_bitmap->Bits(), (void *)kIconBits, icon_bitmap->BitsLength()); // "normal" edition
	//memcpy(icon_bitmap->Bits(), (void *)kDGBheadBits, icon_bitmap->BitsLength()); // "Dominic G" edition
	
	if(selected != NULL)
	{
		// cache a pointer
		name_ptr = selected->AttrName();
		// fill the drag message
		// "name", "type", "addon?", "addon id", "win id"
		drag_msg.AddString("name", name_ptr);
		drag_msg.AddString("type", selected->TypeName());
		drag_msg.AddBool("addon?", selected->HasAddon());
		drag_msg.AddInt32("addon id", selected->Id());
		drag_msg.AddInt32("win id", window->ID());
		// make the drag bitmap
		//printf("StringWidth\n");
		name_width = be_plain_font->StringWidth(name_ptr);
		//printf("ctors\n");
		drag_bitmap = new BBitmap(BRect(0,0,20 + name_width,15), B_RGB_32_BIT, true);
		a_view = new BView(BRect(0,0,20 + name_width, 15), "draw view", B_FOLLOW_ALL, 0);
		//printf("AddChild\n");
		//a_view->SetViewColor(B_TRANSPARENT_32_BIT); // background = clear
		
		//a_view->SetHighColor(0,0,0,255); // high = black
		drag_bitmap->AddChild(a_view);
		//printf("DrawBitmap\n");
		if(drag_bitmap->Lock())
		{	
			a_view->SetHighColor(0, 0, 0, 0); // clear
			a_view->FillRect(a_view->Bounds()); // fill with transparency
			
			// Drawing State magic
			a_view->SetDrawingMode(B_OP_ALPHA);
			a_view->SetHighColor(0, 0, 0, 128);
			a_view->SetBlendingMode(B_CONSTANT_ALPHA,B_ALPHA_COMPOSITE);
			
			a_view->DrawBitmap(icon_bitmap); // blit the icon bitmap
			
			//printf("DrawString\n");
			//a_view->SetViewColor(216,216,216,1); // black background
			//a_view->SetLowColor(0,0,0,255); // low = black
			//a_view->ForceFontAliasing(true);
			//a_view->SetDrawingMode(B_OP_OVER);
			a_view->DrawString(name_ptr, BPoint(20, 12));
			
			//printf("Sync\n");
			a_view->Sync();
			drag_bitmap->Unlock();
		}
		// yahoo
		if(drag_msg.CountNames(B_ANY_TYPE) == 5)
		{
			drag_point.x = mouse_down_point.x;
			drag_point.y = (int32)mouse_down_point.y % ((int32)ItemFrame(CurrentSelection()).Height() + 1);
			DragMessage(&drag_msg, drag_bitmap, B_OP_ALPHA, drag_point);
			//DragMessage(&drag_msg, drag_bitmap, B_OP_BLEND, drag_point);
			currently_dragging = true;
		}
	}
}

bool ColumnListView::InitiateDrag(BPoint point, int32 index, bool wasSelected)
{
	//printf("ColumnListView::InitiateDrag()\n");
	
	mouse_down_point = point;

	if(!wasSelected)
		Select(index); // select it if it wasn't already selected
	
	return true;
}

bool ColumnListView::AddList(BList* newItems)
{
	return AddListPrivate(newItems,CountItems());
}


bool ColumnListView::AddList(BList* newItems, int32 fullListIndex)
{
	return AddListPrivate(newItems,fullListIndex);
}


bool ColumnListView::AddListPrivate(BList* newItems, int32 fullListIndex)
{
	int32 NumberOfItems = newItems->CountItems();
	for(int32 count = 0; count < NumberOfItems; count++)
		if(!AddItem((CLVListItem*)newItems->ItemAt(count),fullListIndex+count))
			return false;
	return true;
}


bool ColumnListView::RemoveItem(CLVListItem* item)
{
	if(item == NULL || !fFullItemList.HasItem(item))
		return false;
	return BListView::RemoveItem((BListItem*)item);
}

CLVListItem* ColumnListView::FullListItemAt(int32 fullListIndex) const
{
	return (CLVListItem*)fFullItemList.ItemAt(fullListIndex);
}


int32 ColumnListView::FullListIndexOf(const CLVListItem* item) const
{
	return fFullItemList.IndexOf((CLVListItem*)item);
}


int32 ColumnListView::FullListIndexOf(BPoint point) const
{
	int32 DisplayListIndex = IndexOf(point);
	CLVListItem* TheItem = (CLVListItem*)ItemAt(DisplayListIndex);
	if(TheItem)
		return FullListIndexOf(TheItem);
	else
		return -1;
}


CLVListItem* ColumnListView::FullListFirstItem() const
{
	return (CLVListItem*)fFullItemList.FirstItem();
}


CLVListItem* ColumnListView::FullListLastItem() const
{
	return (CLVListItem*)fFullItemList.LastItem();
}


bool ColumnListView::FullListHasItem(const CLVListItem* item) const
{
	return fFullItemList.HasItem((CLVListItem*)item);
}


int32 ColumnListView::FullListCountItems() const
{
	return fFullItemList.CountItems();
}


void ColumnListView::MakeEmpty()
{
	fFullItemList.MakeEmpty();
	BListView::MakeEmpty();
}


void ColumnListView::MakeEmptyPrivate()
{
	fFullItemList.MakeEmpty();
	BListView::MakeEmpty();
}


bool ColumnListView::FullListIsEmpty() const
{
	return fFullItemList.IsEmpty();
}


int32 ColumnListView::FullListCurrentSelection(int32 index) const
{
	int32 Selection = CurrentSelection(index);
	CLVListItem* SelectedItem = (CLVListItem*)ItemAt(Selection);
	return FullListIndexOf(SelectedItem);
}