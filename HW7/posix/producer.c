#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
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

	/* 開啟建立共享記憶體文件 */
	shmid = shm_open(path, O_RDWR|O_CREAT, 0666);
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

	/* 刪除shmaddr的映射 */
	if(munmap(shmaddr, shmblocks*sizeof(struct packet)) == -1) {
		perror("munmap");
		exit(EXIT_FAILURE);
	}
	/* 刪除共享記憶體 */
	shm_unlink(path);

	return 0;
}
