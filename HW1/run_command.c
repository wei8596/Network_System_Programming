/*
 * run_command.c :    do the fork, exec stuff, call other functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include "shell.h"

void run_command(char **myArgv) {
    pid_t pid;
    int stat;
	int BACKGROUND = 0;
	/* BACKGROUND = 1 : 背景執行
	 *			  = 0 : 前景執行
	 */
	BACKGROUND = is_background(myArgv);

    /* Create a new child process. */
	pid = fork();

    switch (pid) {

        /* Error. */
        case -1 :
            perror("fork");
            exit(errno);

        /* Parent. */
        default :
			if(BACKGROUND) {  //背景執行
				return;
			}
            /* Wait for child to terminate. */
			if(waitpid(pid, &stat, 0) == -1) {  //On error, -1 is returned
				perror("waitpid");
				exit(errno);
			}
            /* Optional: display exit status.  (See wstat(5).) */
			if(WIFEXITED(stat)) {  //子程序正常終止
				//子程序的退出狀態
				printf("%s exit with %d\n", myArgv[0], WEXITSTATUS(stat));
			}
			else if(WIFSIGNALED(stat)) {  //子程序因收到signal終止
				//導致子程序終止的signal數量
				printf("%s exit with %d\n", myArgv[0], WTERMSIG(stat));
			}
			else if(WIFSTOPPED(stat)) {  //子程序停止執行
				//導致子程序停止執行的signal數量
				printf("%s exit with %d\n", myArgv[0], WSTOPSIG(stat));
			}

            return;

        /* Child. */
        case 0 :
            /* Run command in child process. */
			execvp(myArgv[0], myArgv);

            /* Handle error return from exec */
			perror("execvp");
			exit(errno);
    }
}
