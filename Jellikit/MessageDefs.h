/*

	MessageDefs.h
	John Wiggins 1998

*/

#ifndef MESSAGE_DEFS_H
#define MESSAGE_DEFS_H

enum
{
	DUMMY =						'dumm',
	CONTAINER =					'ctnr',
	ANSWER_YES =				'yeah',
	ANSWER_NO = 				'nyet',
	
	NEW_WINDOW =				'nwin',
	MAKE_FILE_PANELS =			'mkfp',
	BUILD_MIME_MENU =			'bdmm',
	DO_EASTER_EGG =				'egg ',
	ABOUT_BOX_DYING =			'dead',
	
	DO_YOU_SUPPORT_THIS_TYPE =	'doyu',
	CLEAR_YOUR_VIEW =			'klar',
	ATTR_MANAGER_GREET =		'yoyo',
	TYPE_MENU_CHANGE =			'tmch',
	MIME_MENU_CHANGE =			'mmnu',
	BEOS_TYPE_CHANGE =			'btch',
	ERR_ALREADY_EXISTS =		'errx',
	
	IS_FILE_OPEN =				'isop',
	IS_FILE_DIRTY =				'drty',
	NEW_FILE = 					'newf',
	OPEN_FILE =					'open',
	SAVE_FILE =					'save',
	SAVE_AS_FILE =				'svas',
	SAVE_BEFORE_QUITTING =		'sbfq',
	ADD_FILE_TO_LIST =			'aftl',
	GET_PATH_FOR_FILE =			'gpff',
	SET_PATH_FOR_FILE =			'spff',
	
	EDIT_ATTRIBUTE =			'edit',
	MAKE_NEW_ATTRIBUTE =		'natr',
	ADD_NEW_ATTRIBUTE =			'mnat',
	REMOVE_ATTRIBUTE =			'rmat',
	REPLACE_ATTRIBUTE =			'reat',
	DUPLICATE_ATTRIBUTE =		'dupa',
	APPLY_ATTRIBUTE_EDIT =		'apae',
	DRAGGED_ATTRIBUTE =			'drga',
	DRAGGED_ATTRIBUTE_LISTADD =	'dala',
	GET_ATTRIBUTES =			'gats',
	
	WINDOW_REGISTRY_ADD =		'wrad',
	WINDOW_REGISTRY_SUB =		'wsub',
	WINDOW_ADDED =				'wadd',
	WINDOW_CLEANUP =			'wcln',
	WINDOW_SET_TITLE =			'wstl',
	WINDOW_ACTIVATED =			'wact',
	
	ADD_ATTR_OK =				'aaok',
	ADD_ATTR_CANCEL =			'aaca',
};

#endif