/*
 * redirect_out.c   :   check for >
 */

#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "shell.h"
#define STD_OUTPUT 1
#define STD_INPUT  0

/*
 * Look for ">" in myArgv, then redirect output to the file.
 * Returns 0 on success, sets errno and returns -1 on error.
 */
int redirect_out(char ** myArgv) {
	int i = 0;
  	int fd;

  	/* search forward for > */
	while(myArgv[i] != NULL) {
		if(strcmp(myArgv[i], ">") == 0) {
			break;
		}
		++i;
	}

  	if (myArgv[i]) {	/* found ">" in vector. */

    	/* 1) Open file.
    	 * 2) Redirect to use it for output.
    	 * 3) Cleanup / close unneeded file descriptors.
    	 * 4) Remove the ">" and the filename from myArgv.
		 */
		fd = creat(myArgv[i+1], O_WRONLY | O_CREAT);  //開檔 write-only,若不存在則建立
		chmod(myArgv[i+1], 0664);  //更改檔案權限 加0 (8進位) rw_rw_r__
		dup2(fd, STD_OUTPUT);  //將fd複製到標準輸出
		close(fd);  //複製完後關閉

		/* 移除">"和檔案名稱 */
		while(myArgv[i] != NULL) {
			myArgv[i] = NULL;
			++i;
		}
  	}
  	return 0;
}
