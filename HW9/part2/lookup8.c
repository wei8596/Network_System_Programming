/*
 * lookup8 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet TCP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	/* struct sockaddr_in {
	 *     sa_family_t    sin_family; // address family: AF_INET
	 *     in_port_t      sin_port;   // port in network byte order
	 *     struct in_addr sin_addr;   // internet address
	 * };
	 */
	static int sockfd;
	static struct sockaddr_in server;
	/* struct hostent {
	 *     int    h_length;          // length of address
	 *     char **h_addr_list;       // list of addresses
	 *     ...
	 * };
	 */
	struct hostent *host;
	static int first_time = 1;

	if (first_time) {        /* connect to socket ; resource is server name */
		first_time = 0;

		/* Set up destination address. */
		memset((char *)&server, '\0', sizeof(server));
		server.sin_family = AF_INET;
		server.sin_port = PORT;
		host = gethostbyname(resource);  //取得IP地址
		if(host == NULL) {
			DIE("gethostbyname");
		}
		/* 設定IP地址 */
		memcpy((char *)&server.sin_addr, host->h_addr_list[0], host->h_length);

		/* Allocate socket.
	     * int socket(int domain, int type, int protocol);
		 * AF_INET - 透過網路進行資料傳輸，AF_INET使用的是IPv4協定
		 * SOCK_STREAM - 提供一個序列化的連接導向位元流, 可以做位元流傳輸.
		 * 					對應的protocol為TCP.
		 * 0 - 設定socket的協定標準，一般設為0, 讓kernel選擇type對應的默認協議
		 */
		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			DIE("socket");
		}

		/* Connect to the server.
		 * int connect(int sockfd, const struct sockaddr *addr,
		 *         socklen_t addrlen);
		 * addr - 提供關於這個socket的所有資訊
		 * addr_len - addr的大小
		 */
		if(connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
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
