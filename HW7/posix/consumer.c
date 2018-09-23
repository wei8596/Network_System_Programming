#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "packet.h"

int main(int argc, char *argv[]) {
	int shmblocks;
	char *path = "/share_memory";
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

	/* 開啟共享記憶體文件 */
	shmid = shm_open(path, O_RDWR, 0666);
	if(shmid == -1) {
		perror("shm_open");
		exit(EXIT_FAILURE);
	}

	/* 設定共享記憶體文件大小 */
	if(ftruncate(shmid, shmblocks*sizeof(struct packet)) == -1) {
		perror("ftruncate");
		exit(EXIT_FAILURE);
	}

	/* 映射到virtual memory */
	shmaddr = mmap(NULL, shmblocks*sizeof(struct packet),
		PROT_READ|PROT_WRITE, MAP_SHARED, shmid, 0);
	if(shmaddr == MAP_FAILED) {
		perror("mmap");
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

	/* 刪除shmaddr的映射 */
	if(munmap(shmaddr, shmblocks*sizeof(struct packet)) == -1) {
		perror("munmap");
		exit(EXIT_FAILURE);
	}
	/* 刪除共享記憶體 */
	shm_unlink(path);

	return 0;
}
