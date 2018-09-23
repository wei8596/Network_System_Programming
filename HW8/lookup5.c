/*
 * lookup5 : local file ; setup a memory map of the file
 *           Use bsearch. We assume that the words are already
 *           in dictionary order in a file of fixed-size records
 *           of type Dictrec
 *           The name of the file is what is passed as resource
 */

#include <stdlib.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "dict.h"

/*
 * This obscure looking function will be useful with bsearch
 * It compares a word with the word part of a Dictrec
 */

int dict_cmp(const void *a,const void *b) {
	return strcmp((char *)a,((Dictrec *)b)->word);
}

int lookup(Dictrec * sought, const char * resource) {
	static Dictrec * table;
	static int numrec;
	Dictrec * found, temp;
	static int first_time = 1;

	if (first_time) {  /* table ends up pointing at mmap of file */
		int fd;
		long filsiz;

		first_time = 0;

		/* 唯讀權限開字典. */
		fd = open(resource, O_RDONLY, 0666);
		if(fd == -1) {
			DIE("open");
		}

		/* Get record count for building the tree. */
		filsiz = lseek(fd,0L,SEEK_END);
		numrec = filsiz / sizeof(Dictrec);

		/* 將字典檔案memory map到虛擬記憶體中.
		 * PROT_READ - 映射區域可被讀取
		 * MAP_SHARED - 允許其他映射該檔案的程式共享
		 */
		table = (Dictrec *)mmap(
			NULL, sizeof(Dictrec)*numrec, PROT_READ,
			MAP_SHARED,	fd, 0);
		if(table == (Dictrec *)-1) {
			DIE("mmap");
		}
		
		close(fd);
	}

	/* search table using bsearch */
	strcpy(temp.word, sought->word);
	found = bsearch(&temp, table, numrec, sizeof(Dictrec), dict_cmp);

	if (found) {
		strcpy(sought->text,found->text);
		return FOUND;
	}

	return NOTFOUND;
}
