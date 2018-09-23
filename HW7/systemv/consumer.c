#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "packet.h"

int main(int argc, char *argv[]) {
	int shmblocks;
	key_t key;
	int shmid;
	struct packet *shmaddr;

	/* 檢查參數 */
	if(argc != 2) {
		printf("Usage: ./consumer #shmblocks\n");
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

	int id;
	/* 紀錄id, 用來檢查是否有新資料寫入 */
	int id_check[shmblocks];

	/* 初始化設為-1 */
	for(id = 0; id < shmblocks; ++id) {
		memset(shmaddr+(id%shmblocks), -1, sizeof(struct packet));
		id_check[id] = -1;
	}

	struct packet Packet;
	int num = 0;  //紀錄成功讀取的數量
	id = 0;

	/* 無窮迴圈檢查share memory是否有新資料寫入 */
	while(1) {
		memcpy(&Packet, shmaddr+(id%shmblocks), sizeof(struct packet));
		/* 檢查是否有新資料寫入 */
		if(Packet.id != id_check[id%shmblocks]) {
			/* 更新紀錄的id */
			id_check[id%shmblocks] = Packet.id;
			/* 增加成功讀取的數量 */
			++num;

			if(Packet.id == MAX_PACKET-1) {
				/* 讀取完成 */
				break;
			}
		}
		++id;
	}

	/* 印出遺失資料的數量 */
	printf("Lost: %d datas\n", MAX_PACKET-num);

	/* 解除程序對共用記憶體區域的映射 */
	shmdt(shmaddr);
	/* IPC_RMID - 刪除共用記憶體區域 */
	shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL);

	return 0;
}
