/*
 * lookup2 : local file ; setup an in-memory index of words
 *             and pointers into the file. resource is file name
 *             use qsort & bsearch
 */

#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

typedef struct {
    char word[WORD];  /* The word to be looked up */
    long off;         /* Offset into file for word definition */
} Index;

/*
 * This ugly little function can be used by qsort & bsearch
 * It compares the word part of two structures of type Index
 */

int dict_cmp(const void *a,const void *b) {
	return strcmp(((Index *)a)->word,((Index *)b)->word);
}

int lookup(Dictrec * sought, const char * resource) {
	static Index * table;
	Dictrec dr;
	static int numrec;
	int i;
	Index * found,tmp;
	static FILE * in;
	static int first_time = 1;

	if (first_time) { /* set up index */
		first_time = 0;

		/* Open file. */
		in = fopen(resource, "r");
		if(in == NULL) {
			perror("fopen");
			exit(EXIT_FAILURE);
		}

		/* Get number records in file by dividing ending file offset by recsize. */
		long size;

		fseek(in, 0, SEEK_END);  //移至檔案結尾
		size = ftell(in);  //得到目前檔案位置, 等同檔案大小
		numrec = size / (WORD+TEXT);  //計算紀錄數量

		/* Go to the beginning of the file. */
		rewind(in);  //將目前檔案中的位置重設為起始處


		/* Allocate zeroed-out memory: numrec elements of struct Index. */
		table = calloc(sizeof(Index),numrec);

		/* Read the file into the just-allocated array in memory. */
		i = 0;

		while(!feof(in)) {
			fread(table[i].word, 1, WORD, in);  //word
			table[i].off = ftell(in);			//offset
			fread(dr.text, 1, TEXT, in);		//text
			++i;
		}

		/* Sort the table of entry/offset Index structures. */
		qsort(table,numrec,sizeof(Index),dict_cmp);

	} /* end of first-time initialization */

	/* use bsearch to find word in the table; seek & read from file if found. */
	strcpy(tmp.word,sought->word);
	found = bsearch(&tmp,table,numrec,sizeof(Index),dict_cmp);

	/* If found, go to that place in the file, and read the record into the
	 * caller-supplied space. */
	if (found) {
		/* 移動到offset的位置,並讀出text */
		fseek(in, found->off, SEEK_SET);
		fread(sought->text, 1, TEXT, in);

		return FOUND;
	}
	return NOTFOUND;
}
