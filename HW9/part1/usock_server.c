/*
 * usock_server : listen on a Unix socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include "dict.h"

int main(int argc, char **argv) {
	/* struct sockaddr_un {
	 *     sa_family_t sun_family;               // AF_UNIX
	 *     char        sun_path[108];            // pathname
	 * };
	 */
    struct sockaddr_un server, client;
    int sd,cd,n;
    Dictrec tryit;

    if (argc != 3) {
      fprintf(stderr,"Usage : %s <dictionary source>"
          "<Socket name>\n",argv[0]);
      exit(errno);
    }

    /* Setup socket.
     * int socket(int domain, int type, int protocol);
	 * AF_UNIX - 在本機程序與程序間的傳輸, 讓兩個程序共享一個檔案系統(file system)
	 * SOCK_STREAM - 提供一個序列化的連接導向位元流, 可以做位元流傳輸.
	 * 					對應的protocol為TCP.
	 * 0 - 設定socket的協定標準，一般設為0, 讓kernel選擇type對應的默認協議
	 */
	if((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		DIE("socket");
	}
    
    /* Initialize address. */
	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, argv[2]);
	unlink(server.sun_path); /* Otherwise bind could fail */


    /* Name and activate the socket. */

	/* 綁定server的地址 */
	n = sizeof(struct sockaddr_un);
	if((bind(sd, (struct sockaddr*)&server, n)) == -1){
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

					/* Lookup request. */
					switch(lookup(&tryit,argv[1]) ) {
						/* Write response back to client. */
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

				/* Terminate child process.  It is done. */
				exit(0);

			/* Parent continues here. */
			default :
				/* 關閉client連線 */
				close(cd);
				break;
		} /* end fork switch */
    } /* end forever loop */
} /* end main */
