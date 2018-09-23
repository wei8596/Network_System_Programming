/*
 * printdir.c : prints the current directory
 */

#include <sys/param.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	char *dir = NULL;

	/* 獲取相對路徑的最大長度 */
	long pathmaxlen = pathconf(".", _PC_PATH_MAX);

	/* 獲取當前資料夾路徑 */
	dir = getcwd((char *)NULL, pathmaxlen + 1);

	if(dir == NULL) {
		perror("getcwd");
		exit(EXIT_FAILURE);
	}

	printf("%s\n", dir);
	free(dir);  //釋放空間

	return 0;
}
