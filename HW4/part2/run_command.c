/*
 * run_command.c :    do the fork, exec stuff, call other functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"

void run_command(char **myArgv) {
  	pid_t pid;
  	int stat;
  	int run_in_background;

  	/*
   	* Check for background processing.
   	* Do this before fork() as the "&" is removed from the argv array
   	* as a side effect.
   	*/
  	run_in_background = is_background(myArgv);

	set_timer();	//開始計時
  	switch(pid = fork()) {

    	/* Error. */
    	case -1 :
      		perror("fork");
      		exit(errno);

    	/* Parent. */
    	default :
      		if (!run_in_background) {
        		waitpid(pid,&stat,0);	/* Wait for child to terminate. */
				stop_timer();	//停止計時

        		if (WIFEXITED(stat) && WEXITSTATUS(stat)) {
          			fprintf(stderr, "Child %d exited with error status %d: %s\n",
	      				pid, WEXITSTATUS(stat), (char*)strerror(WEXITSTATUS(stat)));

        		} else if (WIFSIGNALED(stat) && WTERMSIG(stat)) {
	  				fprintf (stderr, "Child %d exited due to signal %d: %s\n",
	      				pid, WTERMSIG(stat), (char*)strsignal(WTERMSIG(stat)));
        		}
      		}
      		return;

    	/* Child. */
    	case 0 :

			/* sigprocmask(): 改變目前的signal mask
			 * SIG_SETMASK: 將目前的signal mask設成oldblock指定的signal mask
			 */
			if((sigprocmask(SIG_SETMASK, &oldblock, (sigset_t *)NULL)) != 0) {
				perror("sigprocmask");
				exit(EXIT_FAILURE);
			}

      		/* Redirect input and update argv. */
			redirect_in(myArgv);

      		/* Redirect output and update argv. */
			redirect_out(myArgv);

      		pipe_and_exec(myArgv);
      		exit(errno);
	}
}
