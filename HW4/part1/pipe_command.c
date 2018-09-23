/* 
 * pipe_command.c  :  deal with pipes
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "shell.h"

#define STD_OUTPUT 1
#define STD_INPUT  0

void pipe_and_exec(char **myArgv) {
  	int pipe_argv_index = pipe_present(myArgv);
  	int pipefds[2];
	char **left_argv;
	//char **right_argv;
	int left_argc = 0;

  	switch (pipe_argv_index) {

    	case -1:	/* Pipe at beginning or at end of argv;  See pipe_present(). */
      		fputs ("Missing command next to pipe in commandline.\n", stderr);
      		errno = EINVAL;	/* Note this is NOT shell exit. */
      		break;

    	case 0:	/* No pipe found in argv array or at end of argv array.
			See pipe_present().  Exec with whole given argv array. */
			execvp(myArgv[0], myArgv);
      		break;

    	default:	/* Pipe in the middle of argv array.  See pipe_present(). */

      		/* Split arg vector into two where the pipe symbol was found.
       		 * Terminate first half of vector.
			 */

			/* "|"設為NULL */
			myArgv[pipe_argv_index] = NULL;

			/* 將左半邊的參數拆出來 */
			left_argv = myArgv;
			left_argv = (char **) malloc(sizeof(char *));
			/* wrong code (order problem) */
			//left_argv = (char **) malloc(sizeof(char *));
			//left_argv = myArgv;
			left_argv[left_argc] = myArgv[0];
			++left_argc;

			/* left_argc <= pipe_argv_index
			 * 最後放的是NULL
			 */
			while(left_argc <= pipe_argv_index) {
				left_argv = (char **) realloc(left_argv, sizeof(char *) * (left_argc+1));
				left_argv[left_argc] = myArgv[left_argc];
				++left_argc;
			}

      		/* Create a pipe to bridge the left and right halves of the vector. */

			/* 建立管道 */
			if(pipe(pipefds) == -1) {
				perror("pipe");
				exit(EXIT_FAILURE);
			}

      		/* Create a new process for the right side of the pipe.
       		 * (The left side is the running "parent".)
			 */
      		switch(fork()) {

        		case -1 :
	  				break;

        		/* Talking parent.  Remember this is a child forked from shell. */
        		default :

	  				/* - Redirect output of "parent" through the pipe.
	  				 * - Don't need read side of pipe open.  Write side dup'ed to stdout.
	 	 			 * - Exec the left command.
					 */
					close(pipefds[0]);		//關閉pipe的read
		            close(STD_OUTPUT);	//關閉標準輸出
		            dup(pipefds[1]);			//將fd[1]複製到標準輸出
		            close(pipefds[1]);		//複製完後關閉

					/* exec可以把當前程序替換為一個新程序, 且PID相同 */
		            execvp(left_argv[0], left_argv);
	  				break;

        		/* Listening child. */
        		case 0 :

	  				/* - Redirect input of "child" through pipe.
					  * - Don't need write side of pipe. Read side dup'ed to stdin.
				  	 * - Exec command on right side of pipe and recursively deal with other pipes
					 */
					close(pipefds[1]);		//關閉pipe的write
		            close(STD_INPUT);	//關閉標準輸入
		            dup(pipefds[0]);			//將fd[0]複製到標準輸入
		            close(pipefds[0]);		//複製完後關閉
					 
          			pipe_and_exec(&myArgv[pipe_argv_index+1]);
			}
	}
	perror("Couldn't fork or exec child process");
  	exit(errno);
}
