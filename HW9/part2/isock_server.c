/*
 * isock_server : listen on an internet socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "dict.h"

int sd,cd,n;

/* signal handler */
void handler(int signo);

int main(int argc, char **argv) {
	/* struct sockaddr_in {
	 *     sa_family_t    sin_family; // address family: AF_INET
	 *     in_port_t      sin_port;   // port in network byte order
	 *     struct in_addr sin_addr;   // internet address
	 * };
	 */
	static struct sockaddr_in server, client;
	Dictrec tryit;
	struct sigaction act;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(1);
	}

	/* 設定抓取Ctrl-c signal */
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = handler;
	sigaction(SIGINT, &act, (struct sigaction *)NULL);

	/* Create the socket.
     * int socket(int domain, int type, int protocol);
	 * AF_INET - 透過網路進行資料傳輸，AF_INET使用的是IPv4協定
	 * SOCK_STREAM - 提供一個序列化的連接導向位元流, 可以做位元流傳輸.
	 * 					對應的protocol為TCP.
	 * 0 - 設定socket的協定標準，一般設為0, 讓kernel選擇type對應的默認協議
	 */
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		DIE("socket");
	}

	/* Initialize address. */
	server.sin_family = AF_INET;
	server.sin_port = PORT;  //5678 (dict.h)

	/* 以SO_REUSEADDR選項調用setsockopt().為了允許地址重用,設置on為1 (0 - 禁止地址重用)
	 * (一般不會立即關閉而經歷TIME_WAIT的過程)
	 */
	int on = 1;
	if((setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) == -1) {
		DIE("setsockopt");
	}

	/* Name and activate the socket. */

	/* 綁定server的地址 */
	n = sizeof(struct sockaddr_in);
	if(bind(sd, (struct sockaddr*)&server, n) == -1) {
		DIE("bind");
	}
	/* 設置socket為listen模式(設定server的監聽隊列)
	 * int listen(int sockfd, int backlog);
	 * backlog - 還未完成連線請求的最大數量
	 */
	if(listen(sd, 128) == -1) {
		DIE("listen");
	}

	/* main loop : accept connection; fork a child to have dialogue */
	for (;;) {

		/* Wait for a connection.
		 * block直到有client連線要求
		 * accept()被調用時, 會為該請求產生一個"新"的socket，並把這個請求從監聽隊列剔除
		 * int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
		 * addr - 一樣是描述socket的結構, 不過是一個空容器, 用於儲存接收到的client端相關資訊
		 */
		cd = accept(sd, (struct sockaddr*)&client, &n);
		if(cd == -1) {
			DIE("accept");
		}

		/* Handle new client in a subprocess. */
		switch (fork()) {
			case -1 : 
				DIE("fork");
			case 0 :
				/* 連接的socket由parent負責, child關閉socket descriptor的副本 */
				close (sd);	/* Rendezvous socket is for parent only. */
				/* Get next request. */
				while ((read(cd, &tryit, sizeof(Dictrec))) != -1) {
					/* Lookup the word , handling the different cases appropriately */
					switch(lookup(&tryit,argv[1]) ) {
						/* Write response back to the client. */
						case FOUND:
							write(cd, &tryit, sizeof(Dictrec));
							break;
						case NOTFOUND:
							strcpy(tryit.text, "XXXX");
	                        write(cd, &tryit, sizeof(Dictrec));
							break;
						case UNAVAIL:
							DIE(argv[1]);
					} /* end lookup switch */
				} /* end of client dialog */
				exit(0); /* child does not want to be a parent */

			default :
				/* 關閉client連線 */
				close(cd);
				break;
		} /* end fork switch */
	} /* end forever loop */
} /* end main */

void handler(int signo) {
	close(sd);
	exit(1);
}
