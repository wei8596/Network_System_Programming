/*
 * myls_B043040003.c : Usage: ./myls [option] [directory]
 */

#include <sys/param.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_SIZE 400

char* mypwd(void);
void list_directory( char *dir_path );

/* 檢查指令參數-F -R的flag */
int flag_F = 0, flag_R = 0;
/* 目錄指標 */
DIR *dirp;
struct dirent *dent;

int main( int argc, char* argv[] ) {
	char *dir = NULL;
	struct stat sb;

	/* ./myls */
	if(argc < 2) {
		dir = mypwd();
	}
	else {  /* ./myls [option] [directory] */
		if(argv[1][0]!='-' && stat(argv[1], &sb)==-1) {
			perror("stat");
			exit(EXIT_FAILURE);
		}

		/* ./myls directory */
		if(S_ISDIR(sb.st_mode)) {
			dir = argv[1];
		}
		else {  /* ./myls option */
			dir = mypwd();
		}
	}


	int opt;

	/* 解析指令參數 */
	while((opt = getopt(argc, argv, ":FR")) != -1) {
		switch(opt) {
			case 'F':
				flag_F = 1;
				break;
			case 'R':
				flag_R = 1;
				break;
		}
	}

	/* 列出目錄內容 */
	list_directory(dir);

	return 0;
}

/* 取得相對路徑 */
char* mypwd(void) {
	char *dir = NULL;

	/* 獲取相對路徑的最大長度 */
	long pathmaxlen = pathconf(".", _PC_PATH_MAX);

	/* 獲取當前資料夾路徑 */
	dir = getcwd((char *)NULL, pathmaxlen + 1);

	if(dir == NULL) {
		perror("getcwd");
		exit(EXIT_FAILURE);
	}

	return dir;
}

/* 列出目錄內容 */
void list_directory( char *dir_path ) {
	char *fullpath = NULL;

	/* 取得絕對路徑 */
	fullpath = realpath(dir_path, fullpath);
	if(fullpath) {
		printf("\n%s :\n", fullpath);
	}
	else {
		return;
	}

	/* 開啟目錄 */
	if((dirp = opendir(fullpath)) == NULL) {
		return;
	}

	/* 目錄下資料夾數量 */
	int dir_num = 0;
	/* 紀錄目錄下資料夾名稱 */
	char dir_name[MAX_SIZE][MAX_SIZE];

	/* dir_name初始化 */
	memset(dir_name, 0, sizeof(dir_name));

	/* 讀取目錄下的檔案 */
	while(dent = readdir(dirp)) {
		/* 忽略"."和".." */
		if(!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, "..")) {
			continue;
		}

		printf("%s", dent->d_name);

		/* 指令包含參數F (classify) */
		if(flag_F) {
			struct stat sb;

			/* 取得檔案狀態 */
			stat(dent->d_name, &sb);

			/* 利用st_mode與S_IFMT進行AND運算 */
			switch (sb.st_mode & S_IFMT) {
				//directory
				case S_IFDIR:  printf("/");	break;
				//FIFO/pipe
				case S_IFIFO:  printf("|");	break;
				//symlink
				case S_IFLNK:  printf("@");	break;
				//regular file
				case S_IFREG:
					/* 檢查檔案的使用者權限 X_OK : 可執行 */
					if(access(dent->d_name, X_OK) >= 0) {
						printf("*");
					}
					break;
				//socket
				case S_IFSOCK: printf("=");	break;
           }
		}
		printf("\n");

		/* 記錄目錄下資料夾的絕對路徑 */
		if(flag_R && dent->d_type==DT_DIR) {
			strcpy(dir_name[dir_num], fullpath);
			strcat(dir_name[dir_num], "/");
			strcat(dir_name[dir_num], dent->d_name);
			++dir_num;
		}
	}
	/* 關閉目錄 */
	(void)closedir(dirp);

	/* 指令包含參數R (recursive) */
	if(flag_R){
		int i;
		for(i = 0; i < dir_num; ++i) {
			/* 更改工作目錄 */
			chdir(dir_name[i]);
			/* 遞迴拜訪目錄下的資料夾 */
			list_directory(dir_name[i]);
			/* 返回原目錄 */
			chdir(fullpath);
		}
	}
}
