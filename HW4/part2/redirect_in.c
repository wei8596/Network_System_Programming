/*
 * redirect_in.c  :  check for <
 */

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "shell.h"
#define STD_OUTPUT 1
#define STD_INPUT  0

/*
 * Look for "<" in myArgv, then redirect input to the file.
 * Returns 0 on success, sets errno and returns -1 on error.
 */
int redirect_in(char ** myArgv) {
  	int i = 0;
  	int fd;

  	/* search forward for < */
	while(myArgv[i] != NULL) {
		if(strcmp(myArgv[i], "<") == 0) {
			break;
		}
		++i;
	}

  	if (myArgv[i]) {	/* found "<" in vector. */

    	/* 1) Open file.
     	 * 2) Redirect stdin to use file for input.
   		 * 3) Cleanup / close unneeded file descriptors.
   		 * 4) Remove the "<" and the filename from myArgv.
		 */
		fd = open(myArgv[i+1], O_RDONLY);  //開檔 read-only
		dup2(fd, STD_INPUT);  //將fd複製到標準輸入
		close(fd);  //複製完後關閉

		/* 移除"<"和檔案名稱 */
		while(myArgv[i] != NULL) {
			myArgv[i] = NULL;
			++i;
		}
  	}
  	return 0;
}
