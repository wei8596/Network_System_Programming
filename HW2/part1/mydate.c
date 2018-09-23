/*
 * mydate.c : prints out the day and time
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 200  //timestr字串最大長度

int main(void) {
	time_t t;
	struct tm *tmptr;  //時間結構
	char timestr[SIZE];  //時間字串

	time(&t);  //從1970/1/1到現在的時間
	tmptr = localtime(&t);  //轉換成tm結構
	if(tmptr == NULL) {
		perror("localtime");
		exit(EXIT_FAILURE);
	}

	/* 將tm結構中的時間格式轉換成指定格式的字串 */
	if(strftime(timestr, sizeof(timestr), "%b %d(%a), %Y  %R %p", tmptr) == 0) {
		perror("strftime");
		exit(EXIT_FAILURE);
	}

	printf("%s\n", timestr);

	return 0;
}

