/*
 * fifo_server : listen on a named pipe; do lookup ; reply
 *               down another named pipe, the name of which
 *               will be sent by the client (in cli.id)
 *               argv[1] is the name of the local fil
 *               argv[2] is the name of the "well-known" FIFO
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dict.h"

int main(int argc, char **argv) {
	int read_fd,write_fd;
	Client cli;
	Dictrec tryit;

	if (argc != 3) {
		fprintf(stderr,"Usage : %s <dictionary source> ""<resource / FIFO>\n",argv[0]);
		exit(errno);
	}

	/* Check for existence of dictionary and FIFO (both must exist) */
	if(access(argv[1], F_OK) == -1) {
		printf("\'%s\': 沒有此一檔案\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	if(access(argv[2], F_OK) == -1) {
		printf("\'%s\': 沒有此一FIFO\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	/* Open FIFO for reading (blocks until a client connects)
	 * (直到client連接write才會繼續執行)
	 */
	read_fd = open(argv[2], O_RDONLY);
	if(read_fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	/* Sit in a loop. lookup word sent and send reply down the given FIFO */
	for (;;) {

		/* Read request. (讀要找的單字) */
		read(read_fd, &cli, sizeof(cli));

		/* Get name of reply fifo and attempt to open it. */
		write_fd = open(cli.id, O_WRONLY);
		if(write_fd == -1) {
			perror("open");
			exit(EXIT_FAILURE);
		}

		strcpy(tryit.word, cli.word);	/* Get the word to lookup. */

		/* lookup the word , handling the different cases appropriately */
		switch(lookup(&tryit, argv[1]) ) {
			case FOUND:
				write(write_fd, tryit.text, sizeof(tryit.text));
				break;
			case NOTFOUND:
				write(write_fd, "XXXX", sizeof("XXXX"));
				break;
			case UNAVAIL:				/* Other problem. */
				printf("Error\n");
				close(read_fd);
				close(write_fd);
				exit(EXIT_FAILURE);
		}

		/* close connection to this client (server is stateless) */
		close(write_fd);
	}
}
