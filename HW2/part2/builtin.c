/*
 * builtin.c : check for shell built-in commands
 * structure of file is
 * 1. definition of builtin functions
 * 2. lookup-table
 * 3. definition of is_builtin and do_builtin
*/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/utsname.h>
#include "shell.h"

/****************************************************************************/
/* builtin function definitions                                             */
/****************************************************************************/
static void bi_builtin(char ** argv);	/* "builtin" command tells whether a command is builtin or not. */
static void bi_cd(char **argv) ;		/* "cd" command. */
static void bi_echo(char **argv);		/* "echo" command.  Does not print final <CR> if "-n" encountered. */
static void bi_hostname(char ** argv);	/* "hostname" command. */
static void bi_id(char ** argv);		/* "id" command shows user and group of this process. */
static void bi_pwd(char ** argv);		/* "pwd" command. */
static void bi_quit(char **argv);		/* quit/exit/logout/bye command. */




/****************************************************************************/
/* lookup table                                                             */
/****************************************************************************/

static struct cmd {
  	char * keyword;					/* When this field is argv[0] ... */
  	void (* do_it)(char **);		/* ... this function is executed. */
} inbuilts[] = {
  	{ "builtin",    bi_builtin },   /* List of (argv[0], function) pairs. */
    { "echo",       bi_echo },
    { "quit",       bi_quit },
    { "exit",       bi_quit },
    { "bye",        bi_quit },
    { "logout",     bi_quit },
    { "cd",         bi_cd },
    { "pwd",        bi_pwd },
    { "id",         bi_id },
    { "hostname",   bi_hostname },
    {  NULL,        NULL }          /* NULL terminated. */
};


static void bi_builtin(char ** argv) {
	struct cmd *table;

	/* 從inbuilts中找是否相同 */
	for(table = inbuilts; table->keyword != NULL; ++table) {
		if(strcmp(table->keyword, argv[1]) == 0) {
			printf("%s is a builtin feature.\n", argv[1]);
			return;
		}
	}
	printf("%s is NOT a builtin feature.\n", argv[1]);
}

static void bi_cd(char **argv) {
	/* 將目前的工作目錄改變成argv[1]所指的目錄 */
	if(chdir(argv[1]) == -1) {
		perror("chdir");
	}
}

static void bi_echo(char **argv) {
	int place = atoi(argv[2]);

	if(strcmp(argv[1], "-n")==0 && place>0) {	//echo -n ...
		printf("%s\n", argv[place + 2]);
	}
	else {										//echo ...
		int i;
		for(i = 1; argv[i] != NULL; ++i) {
			printf("%s ", argv[i]);
		}
		printf("\n");
	}
}

static void bi_hostname(char ** argv) {
	struct utsname uts;  //記錄uname回傳的資訊

	if(uname(&uts) == -1) {  //獲取系統資訊
		perror("uname");
		exit(EXIT_FAILURE);
	}

	printf("hostname: %s\n", uts.nodename);
}

static void bi_id(char ** argv) {
	struct passwd *pwUser;
	struct group *pwGroup;

	/* 取得對應ID使用者的結構 */
	pwUser = getpwuid( getuid() );
	pwGroup = getgrgid( getgid() );

	printf("UserID = %d(%s), GroupID = %d(%s)\n", pwUser->pw_uid, pwUser->pw_name 
												, pwGroup->gr_gid, pwGroup->gr_name);
}

static void bi_pwd(char ** argv) {
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
}

static void bi_quit(char **argv) {
	exit(0);
}


/****************************************************************************/
/* is_builtin and do_builtin                                                */
/****************************************************************************/

static struct cmd * this; /* close coupling between is_builtin & do_builtin */

/* Check to see if command is in the inbuilts table above.
Hold handle to it if it is. */
int is_builtin(char *cmd) {
	struct cmd *tableCommand;

  	for (tableCommand = inbuilts ; tableCommand->keyword != NULL; tableCommand++)
    	if (strcmp(tableCommand->keyword,cmd) == 0) {
      		this = tableCommand;
      		return 1;
    	}
  return 0;
}


/* Execute the function corresponding to the builtin cmd found by is_builtin. */
void do_builtin(char **argv) {
	this->do_it(argv);
}
