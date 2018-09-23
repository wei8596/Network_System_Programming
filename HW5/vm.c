/*
 * This problem is to demonstrate the use of an IPC semaphore
 * in coordinating VMs which require simultaneous
 * multiple resource allocations, each requesting more
 * than one of the resource.
 *
 * Allocate a semaphore to represent the resources of
 * a personal computer with 8 CPU cores, 16GB RAM,
 * and 500GB disk space to be spread across all concurrent VMs.
 *
 * VMs are created and destroyed on a first come, first served basis.
 * A VM is created and goes only if there are an adequate number of
 * cores, RAM and disk space to cover the VM,
 * after all other VMs are accounted for. If the VM
 * is a go, decrement the resource count, so that all
 * resources in use are accounted for when the next VM is requested.
 */

#include <sys/types.h>/* For general. */
#include <sys/ipc.h>/* System 5 IPC defs. */
#include <sys/sem.h>/* System 5 IPC semaphore defs.*/
#include <pthread.h>/* Posix threads. */
#include <stdlib.h>	/* Needed for delay to work properly. */
#include <stdint.h>	/* intptr_t */
#include <stdio.h>
#include <errno.h>

#define NUM_THREADS 6			  /* Number of simultaneous requests. */
#define TIME_BTWN_NEW_THREADS 0.5 /* Time between intro of new request. */
#define RUNTIME_RANGE 5.0		  /* Time of longest job. */

/* These define a single semaphore group consisting
 * of three semaphores. All the semaphores in a
 * group can be modified together in a single atomic
 * operation. The first semaphore represents the
 * number of cores available; the second: the
 * number of rams available; the third: the amount
 * of disk in 500 units available.
 */

#define NUM_SEMS_IN_GROUP 3
#define CORE_SEM 0
#define RAM_SEM 1
#define DISK_SEM 2

/* These are used to initialize semaphores with total resources managed. */
#define NUM_CORE   8
#define NUM_RAM   16
#define NUM_DISK 500

#define STRING_SIZE 80

#define FALSE 0
#define TRUE (!FALSE)

/* semctl() function has three or four arguments.
 * When there are four, the fourth has the type union semun.
 * The calling program must define this union as follows:
 */
union semun {
	int val;				/* Value for SETVAL */
	struct semid_ds *buf;	/* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;	/* Array for GETALL, SETALL */
	struct seminfo  *__buf;	/* Buffer for IPC_INFO
                                           (Linux-specific) */
};

/* Define some VMs here. This table specifies how
 * many of each resource is requested per each VM.
 * The values in this table cause contention: for
 * example, the first VM uses 4 cores out of 8 available,
 * making some of the others wait for a core.
 */
struct job {
	int numCore;
	int numRam;
	int numDisk;
} jobTable[] = {
	{ 4,5,125},
	{ 6,5,125},
	{ 2,2,500},
	{ 4,5,300},
	{ 2,8,125},
	{ 2,8,300},
};

/* Number of jobs in the table. */
int numJobs = sizeof(jobTable) / sizeof (struct job);

/* IPC semaphore identifier (semaphore accessed through this).*/
int semid;

void *threadMain(void *);
static int playWithSemaphores(
int semid, int numCore, int numRam,
int numDisk);
int reserve(int semid, struct job thisJob);
int release(int semid, struct job thisJob);
void fractSleep(float time);

/* Main function. Allocate and initialize resources,
 * then spawn threads. (建立threads)
 */
int main(void) {
	/* Array of threads, one per request. */
	pthread_t threads[NUM_THREADS];

	/* Needed to convert constant for semctl call.*/
	int numCore = NUM_CORE;
	int numRam = NUM_RAM;
	int numDisk= NUM_DISK;

	int count;

	/* Allocate a single semaphore group with three semaphores in it.
	 * 建立semaphore(信號)
	 * IPC_PRIVATE (0) - 與該程序沒關係的程序無法訪問該信號
	 * NUM_SEMS_IN_GROUP (3) - 建立3個信號
	 */
	if ((semid = semget(IPC_PRIVATE, NUM_SEMS_IN_GROUP, IPC_CREAT | 0600)) == -1){
		perror ("semget");
		exit (errno);
	}

	/* Initialize each semaphore in the group. */
	union semun arg;
	arg.val = numCore;
	if (semctl(semid, CORE_SEM, SETVAL, arg)) {
		perror ("Error initializing CORE semaphores");
		goto cleanup;
	}
	arg.val = numRam;
	if (semctl(semid, RAM_SEM, SETVAL, arg)) {
		perror ("Error initializing RAM semaphores");
		goto cleanup;
	}
	arg.val = numDisk;
	if (semctl(semid, DISK_SEM, SETVAL, arg)) {
		perror ("Error initializing DISK semaphores");
		goto cleanup;
	}

	/* Spawn the threads. The argument passed to
	 * threadMain is a job table index, so multiple
	 * requests can be made using the same table entry
	 * when the index wraps. Delay to model jobs being
	 * staggered instead of coming all at once.
	 */
	for (count = 0; count < NUM_THREADS; count++) {
		if (pthread_create(&threads[count], NULL, threadMain, (void *)(count % numJobs))) {
			perror ("Error starting reader threads");
			goto cleanup;
		}
		fractSleep(TIME_BTWN_NEW_THREADS);  //delay
	}

	/* Wait for threads to finish.
	 * 主程式等待threads都執行完才繼續執行
	 */
	for (count = 0; count < NUM_THREADS; count++) {
		pthread_join(threads[count], (void **)NULL);
	}

cleanup:

	/* IPC_RMID - Delete the semaphore.
	 * This is not done automatically by the system.
	 */
	if (semctl(semid, 0, IPC_RMID, NULL)) {
		perror ("semctl IPC_RMID:");
	}

	return 0;
}

/* Here is where a thread starts executing. */
void *threadMain(void * arg) {
	/* The argument passed in is a table index.
	 * intptr_t - convert pointer to integer
	 */
	int jobNum = (intptr_t)arg;

	/* Local string for message composition. */
	char string[STRING_SIZE];

	sprintf (string,
	"VM # %d requesting %d core, %d ram, "
	"%d disk\n",
	jobNum, jobTable[jobNum].numCore,
	jobTable[jobNum].numRam,
	jobTable[jobNum].numDisk);

	printf("%s", string);

	/* Get the resources needed. Wait for them if necessary. */
	if (reserve(semid, jobTable[jobNum])) {
		perror ("reserve");
		return (NULL);
	}

	sprintf (string,
	"VM # %d got %d core, %d ram"
	", %d disk and is running\n",
	jobNum, jobTable[jobNum].numCore,
	jobTable[jobNum].numRam,
	jobTable[jobNum].numDisk);

	printf("%s", string);

	/* Delay to simulate the time the resources are in use. */
	fractSleep(0.15 * RUNTIME_RANGE);

	sprintf (string,
	"VM # %d done; returning %d "
	"core, %d ram, %d disk\n",
	jobNum, jobTable[jobNum].numCore,
	jobTable[jobNum].numRam,
	jobTable[jobNum].numDisk);

	printf("%s", string);

	/* Release resources. */
	if (release(semid, jobTable[jobNum])) {
		perror ("release");
	}

	return (NULL);
}

/* This is the workhorse(主力) function that allocates /
 * deallocates resources from the semaphore group.
 */
static int playWithSemaphores(
int semid, int numCore, int numRam,
int numDisk) {
	/* There is one operation per semaphore for this
	 * example. This allocates an array of semaphore
	 * operations, all of which is carried out with a
	 * single atomic operation (system call to semop()).
	 */

	struct sembuf ops[NUM_SEMS_IN_GROUP];

	/* One operation per semaphore. Note that a
	 * negative value to ops[x].sem_op allocates a
	 * resource, while a positive value releases it.
	 */

	ops[0].sem_num = CORE_SEM;	/* semaphore number(0) */
	ops[0].sem_op = numCore;	/* semaphore operation */
	ops[1].sem_num = RAM_SEM;	/* semaphore number(1) */
	ops[1].sem_op = numRam;
	ops[2].sem_num = DISK_SEM;	/* semaphore number(2) */
	ops[2].sem_op = numDisk;

	/* All semaphore operations are to be handled in the same way.
	 * Flags passed thru sembuf structure; wait for resource.
	 * flg為0表示會等待
	 */

	/* operation flags */
	ops[0].sem_flg = ops[1].sem_flg = ops[2].sem_flg = 0;

	/* "The call" that does the work. 操作semaphore */
	return (semop(semid, ops, NUM_SEMS_IN_GROUP));
}

/* Reserve resources required for a job. This
 * wrapper function passes negative values into
 * playWithSemaphores(), since negative values
 * represent resource allocations.
 */
int reserve(int semid, struct job thisJob) {
	return (playWithSemaphores(
		semid,-1*thisJob.numCore, -1*thisJob.numRam,
		-1*thisJob.numDisk));
}

/* Release resources of a job completed. This
 * wrapper function passes positive values into
 * playWithSemaphores(), since positive values
 * represent resource deallocations.
 */
int release(int semid, struct job thisJob) {
	return (playWithSemaphores(
		semid, thisJob.numCore, thisJob.numRam,
		thisJob.numDisk));
}

/* Routine to delay for fractions of a second. */
void fractSleep(float time) {
	static struct timespec timeSpec;

	/* Truncate fraction in int conversion. */
	timeSpec.tv_sec = (time_t)time;

	/* Load fraction. */
	timeSpec.tv_nsec = (int)((time - timeSpec.tv_sec)*
		1000000000);

	/* Delay. */
	nanosleep(&timeSpec, NULL);
}

