/*
 *  pipe_present.c :  check for |
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"

/*
 * Return index offset into argv of where "|" is,
 * -1 if in an illegal position (first or last index in the array),
 * or 0 if not present.
 */
int pipe_present(char ** myCurrentArgv) {
	/* @index : 記錄"|"位置
	 * @argc : 記錄參數個數
	 */
	int index = -1, argc = 0;

  	/* Search through myCurrentArgv for a match on "|". */
	while(myCurrentArgv[argc] != NULL) {
		if(strcmp(myCurrentArgv[argc], "|") == 0 && index == -1) {
			index = argc;
		}
		++argc;
	}

  	if(index == 0 || (index+1) == argc) {
		/* At the beginning or at the end. */
    	return -1;

  	} else if(index == -1) {
		/* Off the end. */
    	return 0;

  	} else {
    	/* In the middle. */
    	return index;
  	}
}
