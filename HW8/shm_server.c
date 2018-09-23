/*
 * shm_server : wait on a semaphore; do lookup ; use shared
 *              memory for communication ; notify using same 
 *              semaphore (client raises by 2; server lowers
 *              by 1 both to wait and to notify)
 *
 *              argv[1] is the name of the local file
 *              argv[2] is the key for semaphores + shared mem
 */

#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"

int main(int argc, char **argv) {
	int shmid,semid;
	long key;
	Dictrec * shm;
	struct stat stbuff;
	extern int errno;
	unsigned short vals[2] = { 1 , 0 };        /* Initial values of semaphores */

	union semun {                        /* Needed for semctl for setup. */
		int             val;
		struct semid_ds *buf;
		unsigned short  *array;
	} setit ;

	struct sembuf wait = {1,-1,SEM_UNDO}; /* used BOTH to wait & notify */

	if (argc != 3) {
		fprintf(stderr,"Usage : %s <dictionary source> <Resource/Key>\n",argv[0]);
		exit(errno);
	}

	/* Create & initialize shared memory & semaphores */

	/* Verify database resource is present. */
	if (stat(argv[1],&stbuff) == -1)
		DIE(argv[1]);

	/* Get key from commandline argument. */
	key = strtol(argv[2],(char **)NULL,0);

	/* Map one record's worth of shared memory.
	 * The word of the sent value will be the request,
	 * and the text of the result will be returned as the answer.
	 */
	shmid = shmget(key, sizeof(Dictrec), IPC_CREAT | 0666);
	if(shmid == -1) {
		DIE("shmget");
	}

	/* Allocate a group of two semaphores.  Use same key as for shmem. */
	semid = semget(key, 2, IPC_CREAT | 0666);  //分配2個semaphores
	if(semid == -1) {
		DIE("semget");
	}

	/* Get shared memory virtual address. */
	shm = (Dictrec *)shmat(shmid, NULL, 0);
	if(shm == (Dictrec *)-1) {
		DIE("shmat");
	}

	/* Set up semaphore group. */
	setit.array = vals;

	/* SETALL - 為所有semaphores初始化 */
	if(semctl(semid, 0, SETALL, setit) == -1) {
		DIE("semctl");
	}

	/* Main working loop */
	for (;;) {

		/* When we are first started, value is zero.  Client sets to two to wake us up.
		 * Try to decrement sem 1.  
		 * Then we will wait here until the semaphore is non-zero
		 */
		/* sem_1減1, 一開始為0無法減, 直到client把sem_1加2,
		 * 才可以減, sem_1變成1, 開始查字典
		 */
		semop(semid, &wait, 1);

		/* Do the lookup here.  Write result directly into shared memory. */
		switch(lookup(shm,argv[1]) ) {
			case FOUND: 
				break;
			case NOTFOUND: 
				strcpy(shm->text,"XXXX");
				break;
			case UNAVAIL:
				DIE(argv[1]);
		}
			
		/* Decrement again so that we will block at the top of the for loop again until a client wakes us up again. */
		/* sem_1減1變成0, 直到client要查字典時才會被喚醒 */
		semop(semid, &wait, 1);

	} /* end for */
}
