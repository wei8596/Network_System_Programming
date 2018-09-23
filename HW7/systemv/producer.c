#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "packet.h"

int main(int argc, char *argv[]) {
	int shmblocks;
	key_t key;
	int shmid;
	struct packet *shmaddr;

	/* 檢查參數 */
	if(argc != 2) {
		printf("Usage: ./producer #shmblocks\n");
		exit(EXIT_FAILURE);
	}

	/* 將讀到的數字字串轉成數字 */
	shmblocks = atoi(argv[1]);
	if(shmblocks <= 0) {
		printf("shmblocks <= 0\n");
		exit(EXIT_FAILURE);
	}

	/* 將檔名與整數轉成一個System V IPC key */
	key = ftok("/etc/passwd", 'J');

	/* 建立一個共用記憶體區域,大小為struct packet的整數倍 */
	shmid = shmget(key,
		shmblocks*sizeof(struct packet),
		IPC_CREAT | 0666);
	if(shmid == -1) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}

	/* 把共用記憶體區域映射到程序空間 */
	shmaddr = (struct packet *)shmat(
		shmid, NULL, 0);
	if(shmaddr == (struct packet *)-1) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}

	/* sleep 15秒 */
	sleep(15);


	int id, i;
	struct packet Packet;
	srand(time(NULL));

	/* 寫資料到share memory */
	for(id = 0; id < MAX_PACKET; ++id) {
		Packet.id = id;  //id: 0~4999

		/* 其餘欄位內容任意給予 */
		for(i = 0; i < 5; ++i) {
			Packet.dataShort[i] = rand()%100;
			Packet.dataLong[i] = rand()%100;
			Packet.dataDouble[i] = rand()%100;
			Packet.dataByte[i] = rand()%100;
		}
		/* 資料第0~shmblocks-1筆依序寫入share memory中,
		 * 第shmblocks筆覆寫到第0筆位置, 依此類推
		 */
		memcpy(shmaddr+(id%shmblocks), &Packet, sizeof(struct packet));
		usleep(1);
	}

	/* 解除程序對共用記憶體區域的映射 */
	shmdt(shmaddr);
	/* IPC_RMID - 刪除共用記憶體區域 */
	shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL);

	return 0;
}
