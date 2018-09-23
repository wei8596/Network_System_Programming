/*
 * mymv_B043040003.c : Usage: ./mymv filel filel2 //This command renames filel to file2
 *						./mymv filel directory //This command renames filel to directory/filel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_SIZE 4096  //設定最大讀寫bytes數

char* mypwd(void);

int main( int argc, char* argv[] ) {
	/* 參數錯誤 */
	if(argc < 3) {
		fputs("Invalid number of arguements\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* 檢查來源檔案是否存在 (F_OK) */
	if(access(argv[1], F_OK) == -1){
		printf("mymv: \'%s\': 沒有此一檔案\n", argv[1]);
		exit(EXIT_FAILURE);
	}


	int fd1;

	/* 開啟來源檔案 */
	fd1 = open(argv[1], O_RDONLY);
	if(fd1 == -1){
		perror("open");
		exit(EXIT_FAILURE);
	}

	struct stat sb;
	/* 紀錄第二個參數是否為資料夾 */
	int is_dir = 0;
	/* @source	 :	目前路徑
	 * @dest	 :	目的路徑
	 * @fileName :	檔案名稱
	 */
	char *source = NULL, *dest = NULL, *fileName = NULL;

	/* 檢查第二個參數是否為資料夾 */
	if(stat(argv[2], &sb) == 0) {
		/* 記錄目前路徑,方便刪除檔案 */
		source = mypwd();
		if(S_ISDIR(sb.st_mode)) {
			is_dir = 1;
			/* 設定目的路徑 */
			dest = argv[2];
			/* 檔案名稱不變 */
			fileName = malloc(sizeof(strlen(argv[1])+1));
			strcpy(fileName, argv[1]);

			/* 更換工作目錄 */
			if(chdir(dest) < 0) {
				perror("chdir");
				exit(EXIT_FAILURE);
			}
		}
	}

	/* 第二個參數不為資料夾, 為檔案名稱 */
	if(!is_dir) {
		fileName = malloc(sizeof(strlen(argv[2])+1));
		strcpy(fileName, argv[2]);
	}

	int fd2;

	/* 檔案已存在, 詢問使用者是否要覆寫 */
	if(access(fileName, F_OK) == 0) {
		char overwrite;
		printf("mymv: overwrite '%s'?", fileName);
		scanf("%c", &overwrite);
		switch(overwrite) {
			case 'Y':
			case 'y':
				fd2 = open(fileName, O_WRONLY | O_TRUNC, 0774);
				if(fd2 == -1){
					perror("open");
					exit(EXIT_FAILURE);
				}
				break;
			default:
				exit(EXIT_FAILURE);
		}
	}
	else {  /* 檔案不存在, 建立檔案 */
		fd2 = open(fileName, O_WRONLY | O_CREAT, 0774);
		if(fd2 == -1){
			perror("open");
			exit(EXIT_FAILURE);
		}
	}

	size_t count;
	char buf[MAX_SIZE];

	/* 複製fd1檔案內容到fd2 */
	while((count = read(fd1, buf, MAX_SIZE)) > 0) {
		if(write(fd2, buf, count) == -1) {
			perror("write");
			exit(EXIT_FAILURE);
		}
	}

	/* 若第二個參數是資料夾, 回到原目錄 */
	if(is_dir) {
		if(chdir(source) < 0) {
			perror("chdir");
			exit(EXIT_FAILURE);
		}
	}

	/* 移除原檔案 */
	if(remove(argv[1]) == -1) {
		perror("remove");
		exit(EXIT_FAILURE);
	}

	return 0;
}

/* 取得相對路徑 */
char* mypwd(void) {
	char *dir = NULL;

	/* 獲取相對路徑的最大長度 */
	long pathmaxlen = pathconf(".", _PC_PATH_MAX);

	/* 獲取當前資料夾路徑 + '\0' */
	dir = (char *)malloc(pathmaxlen + 1);
	strcpy(dir, getcwd((char *)NULL, pathmaxlen+1));

	if(dir == NULL) {
		perror("getcwd");
		exit(EXIT_FAILURE);
	}

	return dir;
}
