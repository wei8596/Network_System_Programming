/*
 * lookup9 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet UDP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
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

	if (first_time) {  /* Set up server address & create local UDP socket */
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

		/* Allocate a socket.
		 * int socket(int domain, int type, int protocol);
		 * AF_INET - 透過網路進行資料傳輸，AF_INET使用的是IPv4協定
		 * SOCK_DGRAM - 提供一個一個的資料包(datagram), 對應的protocol為UDP
		 * 0 - 設定socket的協定標準，一般設為0, 讓kernel選擇type對應的默認協議
		 */
		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			DIE("socket");
		}
	}

	/* Send a datagram & await reply
	 * ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
     * 				const struct sockaddr *dest_addr, socklen_t addrlen);
	 * ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
	 *              struct sockaddr *src_addr, socklen_t *addrlen);
	 * flags - 一般設為0
	 * dest_addr - 目的位址資訊
	 * src_addr - 來源位址資訊
	 */
	int siz;
	sendto(sockfd, sought, sizeof(Dictrec), 0, (struct sockaddr *)&server, sizeof(server));
	recvfrom(sockfd, sought, sizeof(Dictrec), 0, (struct sockaddr *)&server, &siz);

	if (strcmp(sought->text,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
