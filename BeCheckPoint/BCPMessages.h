/*
	
	BCPMessages.h
	John Wiggins 1999

*/

#ifndef BCP_MESSAGES_H
#define BCP_MESSAGES_H

enum {
DUMMY =						'dumm',
BUTTON_PRESS =				'pres',
SHAKE_WINDOW =				'shwi',
PASSWORD_ATTEMPT =			'pwat',
PASSWORD_CHANGE =			'pwch',
CHANGE_PASSWORD_DIALOG =	'cpwd',
WRONG_PASSWORD =			'badp' /* not a message constant. used as a return value */
};

#endif