/*
 * lookup1 : straight linear search through a local file
 * 	         of fixed length records. The file name is passed
 *	         as resource.
 */
#include <string.h>
#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	Dictrec dr;
	static FILE * in;
	static int first_time = 1;

	if (first_time) { 
		first_time = 0;
		/* open up the file */
		if ((in =fopen(resource,"r")) == NULL){DIE(resource);}
	}

	/* read from top of file, looking for match */
	rewind(in);  //設定檔案指標指到檔案的最前面
	while(!feof(in)) {
		fread(dr.word, 1, WORD, in);
		fread(dr.text, 1, TEXT, in);

		/* 在字典中找到單字,並複製資料 */
		if(strcmp(dr.word, sought->word) == 0) {
			strcpy(sought->word, dr.word);
			strcpy(sought->text, dr.text);
			return FOUND;
		}
	}

	return NOTFOUND;  //在字典中沒有找到單字
}
