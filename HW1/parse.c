/*
 * parse.c : use whitespace to tokenise a line
 * Initialise a vector big enough
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"

/* Parse a commandline string into an argv array. */
char ** parse(char *line) {

  	static char delim[] = " \t\n"; /* SPACE or TAB or NL */
  	int count = 0;	//argc
  	char * token;
  	char **newArgv;

  	/* Nothing entered. */
  	if (line == NULL) {
    	return NULL;
  	}

  	/* Init strtok with commandline, then get first token.
     * Return NULL if no tokens in line.
     */
	token = strtok(line, delim);
	if(token == NULL) {  //輸入為delimiters(分隔符)
		return NULL;
	}


  	/* Create array with room for first token. */
	newArgv = (char **) malloc(sizeof(char *));
	newArgv[count] = token;


  	/* While there are more tokens...
	 *
	 *  - Get next token.
	 *	- Resize array.
	 *  - Give token its own memory, then install it.
	 */
	while(token) {
		printf("[%d] : %s\n", count, newArgv[count]);	/* 印出token */
		++count;
		token = strtok(NULL, delim);
		newArgv = (char **) realloc(newArgv, sizeof(char *) * (count+1));
		newArgv[count] = token;
	}


  	/* Null terminate the array and return it. */
	if(token == NULL) {
		return newArgv;
	}

  	return newArgv;
}


/*
 * Free memory associated with argv array passed in.
 * Argv array is assumed created with parse() above.
 */
void free_argv(char **oldArgv) {
	/* Free each string hanging off the array.
	 * Free the oldArgv array itself.
	 */
	free(oldArgv);
	oldArgv = NULL;
}
