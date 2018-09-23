/*
 * udp_server : listen on a UDP socket ;reply immediately
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

#include "dict.h"

int main(int argc, char **argv) {
	/* struct sockaddr_in {
	 *     sa_family_t    sin_family; // address family: AF_INET
	 *     in_port_t      sin_port;   // port in network byte order
	 *     struct in_addr sin_addr;   // internet address
	 * };
	 */
	static struct sockaddr_in server,client;
	int sockfd,siz;
	Dictrec dr, *tryit = &dr;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(errno);
	}

	/* Create a UDP socket.
	 * int socket(int domain, int type, int protocol);
	 * AF_INET - 透過網路進行資料傳輸，AF_INET使用的是IPv4協定
	 * SOCK_DGRAM - 提供一個一個的資料包(datagram), 對應的protocol為UDP
	 * 0 - 設定socket的協定標準，一般設為0, 讓kernel選擇type對應的默認協議
	 */
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		DIE("socket");
	}

	/* Initialize address. */
	server.sin_family = AF_INET;
	server.sin_port = PORT;  //5678 (dict.h)


	/* Name and activate the socket. */
	/* 綁定server的地址 */
	siz = sizeof(server);
	if(bind(sockfd, (struct sockaddr*)&server, siz) == -1) {
		DIE("bind");
	}
	/* UDP不用listen */

	for (;;) { /* await client packet; respond immediately */

		siz = sizeof(client); /* siz must be non-zero */

		/* Wait for a request.
		 * ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
		 *              struct sockaddr *src_addr, socklen_t *addrlen);
		 * flags - 一般設為0
		 * src_addr - 來源位址資訊
		 */

		while ((recvfrom(sockfd,tryit,sizeof(Dictrec),0,(struct sockaddr *)&client,&siz)) >= 0) {
			/* Lookup request and respond to user. */
			switch(lookup(tryit,argv[1]) ) {
				case FOUND: 
					/* Send response.
					 * ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                     * 				const struct sockaddr *dest_addr, socklen_t addrlen);
					 * flags - 一般設為0
					 * dest_addr - 目的位址資訊
					 */
					sendto(sockfd, tryit, sizeof(Dictrec), 0, (struct sockaddr *)&client, siz);
					break;
				case NOTFOUND : 
					/* Send response. */
					strcpy(tryit->text, "XXXX");
					sendto(sockfd, tryit, sizeof(Dictrec), 0, (struct sockaddr *)&client, siz);
					break;
				case UNAVAIL:
					DIE(argv[1]);
			} /* end lookup switch */
		} /* end while */
	} /* end forever loop */
} /* end main */
