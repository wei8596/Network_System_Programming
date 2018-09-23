#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

#define SHM_SIZE 2048

/* Define a small stack size to cause contention. */
#define STACK_SIZE 3

/* Define the data structure shared between the
processes. */

static char buffer[STACK_SIZE];/* Stack’s buffer */
static int *myBuf;
static int *stack_index;	   /* Stack’s index. */
static sem_t *Scount;		   /* semaphore */

void push(char oneChar);
char pop(void);

int main(void) {
	int shmid, i, *shmaddr;
	int pid;
	char sem1[] = "/mysemaphore";
	for(i = 0; i < STACK_SIZE; ++i) {
		buffer[i] = '\0';
	}
	/* 映射到virtual memory,可讓parent和child共用 */
	stack_index = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	*stack_index = 0;

	/* 建立一個共用記憶體區域 */
	shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
	if(shmid == -1) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}

	/* 把共用記憶體區域映射到程序空間 */
	shmaddr = (int *)shmat(shmid, NULL, 0);
	if(shmaddr == (int *)-1) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}

	/* myBuf references shared mem */
	myBuf = shmaddr;
	/* copy the buffer into shared memory */
	memcpy (myBuf, &buffer, sizeof(buffer));
	/* 建立一個有名稱的semaphore,數值設為1 */
	Scount = sem_open(sem1, O_CREAT, 0666, 1);

	char temp[STACK_SIZE];	//給parent和child儲存變數
	srand(time(NULL));		//設定亂數種子

	pid = fork();			//fork
	if(pid < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	//child
	else if(pid == 0) {
		sprintf(temp, "123");
	}
	//parent
	else {
		sprintf(temp, "ABC");
	}

	//push and pop
	for(i = 0; i < STACK_SIZE; ++i) {
		sleep(rand()%3+1);
		push(temp[i]);
		sleep(rand()%3+1);
		pop();
	}

	//child is done.
	if(pid == 0) {
		return 0;
	}
	//The parent needs to wait for the children (at least 1) to finish
	wait(0);
	/* 關閉有名稱的semaphore */
	sem_close(Scount);
	printf("\nParent removing shared memory\n");
	/* 移除有名稱的semaphore */
	sem_unlink(sem1);
	/* 解除程序對共用記憶體區域的映射 */
	shmdt(shmaddr);
	/* IPC_RMID - 刪除共用記憶體區域 */
	shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL);
	/* 刪除stack_index的映射 */
	munmap(stack_index, sizeof(*stack_index));

	return 0;
}

/* Push a character onto the stack. Return in the
 * second argument the stack index corresponding to
 * where the character is pushed.
 */
void push(char oneChar) {
	char string[25];

	/* 將semaphore - 1,若semaphore為0
	 * 則等待直到semaphore > 0
	 */
	sem_wait(Scount);

	/* Test if the stack is pushable. */
	while(*stack_index==STACK_SIZE || myBuf[*stack_index]!='\0') {
		//printf("push sleeping...\n");
		sleep(rand()%2+1);
	}

	/* Stack is pushable. Push the data. */
	myBuf[*stack_index] = oneChar;
	++(*stack_index);

	sprintf (string,"Push:\tchar %c\tindex %d\n",
		oneChar, (*stack_index)-1);
	printf("%s", string);

	/* 將semaphore + 1 */
	sem_post(Scount);
}

/* Pop a character from the stack. Return in the
 * second argument the stack index corresponding to
 * where the character is popped.
 */
char pop(void) {
	char toReturn;
	char string[25];

	/* 將semaphore - 1,若semaphore為0
	 * 則等待直到semaphore > 0
	 */
	sem_wait(Scount);

	/* Test if the stack is poppable. */
	while(*stack_index == 0) {
		//printf("pop sleeping...\n");
		sleep(rand()%2+1);
	}

	/* Stack is poppable. Pop the data. */
	--(*stack_index);
	toReturn = myBuf[*stack_index];
	myBuf[*stack_index] = '\0';

	sprintf (string, "Pop:\tchar %c\tindex %d\n",
		toReturn, *stack_index);
	printf("%s", string);

	/* 將semaphore + 1 */
	sem_post(Scount);

	return toReturn;
}
