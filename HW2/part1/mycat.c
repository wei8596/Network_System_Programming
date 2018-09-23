/*
 * mycat.c : a simple version of the program cat
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
	/* 檢查是否只有一個參數 (argc = 2) */
	if(argc != 2) {
		fprintf(stderr, "Usage: mycat filename\n");
		exit(EXIT_FAILURE);
	}


	int fd;

	/* 獲取file descriptor
	 * read-only
	 */
	fd = open(argv[1], O_RDONLY);
	if(fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	char ch;

	/* 每次從檔案讀一個字元並印出, 直到檔案結尾 */
	while( read(fd, &ch, 1) ) {
		printf("%c", ch);
	}

	close(fd);  //關閉檔案

	return 0;
}
