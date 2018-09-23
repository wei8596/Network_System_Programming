/*
 * lookup4 : This file does no looking up locally, but instead asks
 * a server for the answer. Communication is by message queues.
 * The protocol is : messages of type 1 are meant for the server.
 * The client only reads messages whose type matches their own pid.
 * The message queue key is what is passed as resource.
 */
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static ClientMessage snd;
	ServerMessage rcv;
	static int qid;
	static int pid;
	static int first_time = 1;

	if (first_time) { /* open message queue */
		first_time = 0;

		/* Prepare our ClientMessage structure. */
		pid = getpid();
		sprintf(snd.content.id,"%d",pid);
		snd.type = 1L;

		/* Open the message queue.  Use resource pointer value as key.
		 * strtol() - 將字串轉為長整數
		 * 當base為0時,預設採用10進制轉換,
		 * 但如果遇到'0x'/'0X'前置字元則會使用16進制轉換
		 */
		long int key = strtol(resource, NULL, 0);
		qid = msgget(key, IPC_CREAT | 0660);
		if(qid == -1) {
			perror("megget");
			exit(EXIT_FAILURE);
		}
	}

	/* Send server the word to be found ; await reply */
	strcpy(snd.content.word, sought->word);
	if(msgsnd(qid, &snd, sizeof(snd), 0) == -1) {	//送到message queue
			perror("msgsnd");
			exit(EXIT_FAILURE);
	}

	/* 從message queue讀
	 * msgtyp>0 - 回傳第一項msgtyp(pid)與struct msgbuf中的mtype相同的message
	 * MSG_NOERROR - 若取得的message長度大於sizeof(rcv),
	 * 只返回sizeof(rcv)長度的messages,剩下的部分捨棄
	 */
	if(msgrcv(qid, &rcv, sizeof(rcv), pid, MSG_NOERROR) == -1) {
			perror("msgrcv");
			exit(EXIT_FAILURE);
	}

	strcpy(sought->text,rcv.text);

	/* Server returns XXXX when it cannot find request. */
	if (strcmp(rcv.text,"XXXX") != 0)
		return FOUND;

	return NOTFOUND;
}
