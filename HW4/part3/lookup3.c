/*
 * lookup3 : This file does no looking up locally, but instead asks
 * a server for the answer. Communication is by named pipes.
 * The server sits listening on a "well-known" named pipe
 * (this is what is passed to lookup3 as resource)
 * The Client sets up a FIFO for reply, and bundles the FIFO
 * name with the word to be looked up. (See Client in dict.h)
 * Care must be taken to avoid deadly embrace (client & server
 * both waiting for something which can never happen)
 */

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dict.h"

static Client me;

void cleanup(const char * resource) {
	/* Delete named pipe from system.
	 * unlink() - 刪除參數指定的文件
	 * 如果該文件名為最後連接點,但有其他程序打開了此文件,
	 * 則在所有關於此文件的file descriptor皆關閉後才會刪除
	 */
	unlink(resource);
}

int lookup(Dictrec * sought, const char * resource) {
	static int write_fd;
	int read_fd;
	static int first_time = 1;

	if (first_time) {
		first_time = 0;

		/* Open existing named pipe for client->server communication. */
		write_fd = open(resource, O_WRONLY);
		if(write_fd == -1) {
			perror("open");
			exit(EXIT_FAILURE);
		}

		/* Create temporary named pipe for server->client communication. */
		umask(0);	//設定權限 (~0) --> 777
		tmpnam(me.id);	//建立臨時檔案
		if(mkfifo(me.id, 0666) == -1) {
			DIE(me.id);
		}

		/* Register cleanup handler. */
		cleanup(resource);
	}

	/* Send server the word to be found ; await reply */
	strcpy(me.word,sought->word);
	write(write_fd, &me, sizeof(me));

	/* Open the temporary named pipe to get the answer from it. */
	read_fd = open(me.id, O_RDONLY);
	if(read_fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	/* Get the answer. */
	char buf[TEXT];
	read(read_fd, &buf, sizeof(buf));
	strcpy(sought->text, buf);

	/* Close the temporary named pipe as done. */
	close(read_fd);

	/* If word returned is not XXXX it was found. */
	if (strcmp(sought->text,"XXXX") != 0)
		return FOUND;

	return NOTFOUND;
}
