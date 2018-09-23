/*
 * is_background.c :  check for & at end
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"

int is_background(char ** myArgv) {

  	if (*myArgv == NULL)
    	return 0;

  	/* Look for "&" in myArgv, and process it.
  	 *
	 *	- Return TRUE if found.
	 *	- Return FALSE if not found.
	 */
	int argc = 0;

	while(myArgv[argc] != NULL) {
		if(strcmp(myArgv[argc], "&") == 0) {	//found
			myArgv[argc] = NULL;
			return TRUE;
		}
		++argc;
	}
	return FALSE;	//not found
}
