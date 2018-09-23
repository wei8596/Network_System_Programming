/*
 * pipe_ls.c : practice using pipe() and dup()
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STD_INPUT 0
#define STD_OUTPUT 1

int main(void) {
	pid_t pid;
	char *myArgv1[] = {"ls", "-al", NULL};
	char *myArgv2[] = {"cat", NULL};

	/* 儲存pipe的file descriptor
	 * fd[0] for read, fd[1] for write
	 */
	int fd[2];

	/* 建立管道 */
	if(pipe(fd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	/* Create a new child process. */
	pid = fork();

    switch (pid) {

        /* Error. */
        case -1 :
            perror("fork");
            exit(EXIT_FAILURE);

		/* Child. (Write) */
        case 0 :
			close(fd[0]);		//關閉pipe的read
            close(STD_OUTPUT);	//關閉標準輸出
            dup(fd[1]);			//將fd[1]複製到標準輸出
            close(fd[1]);		//複製完後關閉

			/* exec可以把當前程序替換為一個新程序, 且PID相同
			 * 執行 ls -al
			 */
            execvp(myArgv1[0], myArgv1);

			/* Handle error return from exec */
			perror("execvp");
			exit(EXIT_FAILURE);

		/* Parent. (Read) */
        default :
			close(fd[1]);		//關閉pipe的write
            close(STD_INPUT);	//關閉標準輸入
            dup(fd[0]);			//將fd[0]複製到標準輸入
            close(fd[0]);		//複製完後關閉

			/* exec可以把當前程序替換為一個新程序, 且PID相同
			 * 執行 cat
			 */
            execvp(myArgv2[0], myArgv2);

            /* Handle error return from exec */
			perror("execvp");
			exit(EXIT_FAILURE);
    }

	return 0;
}
