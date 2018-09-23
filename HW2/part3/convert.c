/*
 * convert.c : take a file in the form 
 *  word1
 *  multiline definition of word1
 *  stretching over several lines, 
 * followed by a blank line
 * word2....etc
 * convert into a file of fixed-length records
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "dict.h"
#define BIGLINE 512

int main(int argc, char **argv) {
	FILE *in;
	FILE *out;        /* defaults */
	char line[BIGLINE];
	static Dictrec dr;
	//static Dictrec blank;
	
	/* If args are supplied, argv[1] is for input, argv[2] for output */
	if (argc==3) {
		if ((in =fopen(argv[1],"r")) == NULL){DIE(argv[1]);}
		if ((out =fopen(argv[2],"w")) == NULL){DIE(argv[2]);}	
	}
	else{
		printf("Usage: convert [input file] [output file].\n");
		return -1;
	}

	/* Main reading loop : read word first, then definition into dr */
	
	/* 宣告getline所需的變數 */
	ssize_t size;
	char *str;
	size_t n = 0;  //須初始化為0

	/* Loop through the whole file. */
	while (!feof(in)) {
		
		/* Create and fill in a new blank record.
		 * First get a word and put it in the word field, then get the definition
		 * and put it in the text field at the right offset.  Pad the unused chars
		 * in both fields with nulls.
		 */

		/* 全部設為NULL (char)0 */
		memset(&dr, 0, BIGLINE);
		memset(line, 0, BIGLINE);

		/* Read word and put in record.  Truncate at the end of the "word" field. */

		/* ssize_t getline(char **lineptr, size_t *n, FILE *stream); */
		size = getline(&str, &n, in);

		/* EOF */
		if(size == -1) {
			break;
		}

		strcpy(dr.word, str);  //複製str到dr.word
		dr.word[size - 1] = '\0';  //'\n'改成'\0'

		/* Read definition, line by line, and put in record. */
		int text_size = 0;  //記錄單字定義的長度

		while(1) {
			size = getline(&str, &n, in);
			/* 單字定義結束 (EOF 或 換行) */
			if(size == -1 || str[0] == '\n') {
				dr.text[text_size] = '\n';
				break;
			}

			str[size - 1] = '\0';  //'\n'改為'\0'

			/* 定義超過一行 */
			if(text_size) {
				strcat(dr.text, str);
			}
			/* 第一行定義 */
			else {
				strcpy(dr.text, str);
			}
			text_size += (size - 1);  //扣除計算'\n'
		}

		/* Write record out to file. */

		/* 把word與text複製到line, 並寫到檔案 */
		strcpy(line, dr.word);
		strcpy(line+WORD, dr.text);
		fwrite(line, 1, BIGLINE, out);
	}

	fclose(in);
	fclose(out);
	return 0;
}
