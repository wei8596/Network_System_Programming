/*
 * hostinfo.c : prints out system information
 */

#include <sys/utsname.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    struct utsname uts;  //記錄uname回傳的資訊

	if(uname(&uts) == -1) {  //獲取系統資訊
		perror("uname");
		exit(EXIT_FAILURE);
	}

	printf("hostname: %s\n", uts.nodename);
	printf("Linux %s\n", uts.release);
	printf("hostid: %ld\n", gethostid());  //回傳值為長整數型態

	return 0;
}

