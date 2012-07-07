/*

	AttributeManager.h
	John Wiggins 1998

*/

#ifndef ATTRIBUTE_MANAGER_H
#define ATTRIBUTE_MANAGER_H

// app kit
#include <Looper.h>
#include <Message.h>
#include <Invoker.h>
// interface kit
#include <Alert.h>
// support kit
#include <List.h>
#include <SupportDefs.h>
// ansi
#include <string.h>
// mine
#include "MessageDefs.h"
#include "EditorWindow.h"
#include "AttrWindow.h"
#include "AttrApp.h"
#include "AppResStrings.h"
#include "AppResStringDefs.h"

class EditorWindow;
class BWindow;
class BList;

struct file_list_entry {
	int32 window_id; // what window are these attributes for?
	char *filename; // what filename are these attributes for?
	BMessage *attributes; // what are the attributes?
	BMessage *remove_list; // which attrs, if any, need to be removed?
	bool file_is_dirty;
};

class AttributeManager : public BLooper {
public:
						AttributeManager(EditorWindow *window);
						~AttributeManager();
virtual	void			MessageReceived(BMessage *msg);

private:
file_list_entry			*GetListItem(int32 window_id);
bool					IsFileOpen(const char *path);
void					AddFileToList(const char *path, BMessage *container, int32 window_id);
void					GrabAttribute(const char *name, int32 window_id, const void **attribute, ssize_t *size, type_code *type);
void					ReplaceAttribute(const char *name, const void *attribute, type_code type, ssize_t size, int32 window_id);
void					GetPathForFile(int32 window_id, char **pathname);
void					SetPathForFile(int32 window_id, const char *pathname);
status_t				CopyAttrWinToWin(int32 from_win, int32 to_win, const char *name, bool overwrite);
status_t				AddNewAttrToWin(const char *name, const void *data, type_code type, ssize_t size, int32 window_id);
BWindow					*FindWindowByID(int32 window_id);

// data
BList					*file_list;
EditorWindow			*editor_window;
};

#endif
