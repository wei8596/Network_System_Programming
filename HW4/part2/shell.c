/*
 * shell.c  : test harness for parse routine
 */

#define LONGLINE 255

#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include <signal.h>

int main(int argc, char* argv[]) {
	char line[LONGLINE];
  	char **myArgv;

	/* parent忽略control-C和control-'\' */

	/* Initialize toblock to all 0's */
	sigemptyset(&toblock);

	sigaddset(&toblock, SIGINT);	//control-C
	sigaddset(&toblock, SIGQUIT);	//control-'\'

	/* sigprocmask(): 改變目前的signal mask
	 * SIG_BLOCK: 新的signal mask由目前的signal mask和toblock指定的signal mask做聯集
	 */
	if((sigprocmask(SIG_BLOCK, &toblock, &oldblock)) != 0) {
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

  	fputs("myshell -> ",stdout);
  	while (fgets(line,LONGLINE,stdin)) {

    	/* Create argv array based on commandline. */
    	if ((myArgv = parse(line))!= NULL) {

      		/* If command is recognized as a builtin, do it. */
      		if (is_builtin(myArgv[0])) {
        		do_builtin(myArgv);

			/* Non-builtin command. */
			} else {
				run_command(myArgv);
			}

			/* Free argv array. */
			free_argv(myArgv);
		}

    	fputs("myshell -> ",stdout);
	}
  	exit(0);
}
