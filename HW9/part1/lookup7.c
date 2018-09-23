/*
 * lookup7 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Unix TCP Sockets
 * The name of the socket is passed as resource.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	/* struct sockaddr_un {
	 *     sa_family_t sun_family;               // AF_UNIX
	 *     char        sun_path[108];            // pathname
	 * };
	 */
	static int sockfd;
	static struct sockaddr_un server;
	static int first_time = 1;
	int n;

	if (first_time) {     /* connect to socket ; resource is socket name */
		first_time = 0;

		/* Set up destination address. */
		server.sun_family = AF_UNIX;
		strcpy(server.sun_path,resource);

		/* Allocate socket.
		 * int socket(int domain, int type, int protocol);
		 * AF_UNIX - 在本機程序與程序間的傳輸, 讓兩個程序共享一個檔案系統(file system)
		 * SOCK_STREAM - 提供一個序列化的連接導向位元流, 可以做位元流傳輸.
		 * 					對應的protocol為TCP.
		 * 0 - 設定socket的協定標準，一般設為0, 讓kernel選擇type對應的默認協議
		 */
		if((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
			DIE("socket");
		}

		/* Connect to the server.
		 * int connect(int sockfd, const struct sockaddr *addr,
		 *         socklen_t addrlen);
		 * addr - 提供關於這個socket的所有資訊
		 * addr_len - addr的大小
		 */
		n = sizeof(struct sockaddr_un);
		if(connect(sockfd, (struct sockaddr *)&server, n) == -1) {
			DIE("connect");
		}
	}

	/* write query on socket ; await reply */
	write(sockfd, sought, sizeof(Dictrec));
	read(sockfd, sought, sizeof(Dictrec));

	if (strcmp(sought->text,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
