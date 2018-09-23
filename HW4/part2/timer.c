/*
 *  timer.c : contains two timing functions to measure process time
 */

#include <sys/times.h>
#include <unistd.h>
#include <stdio.h>
#include "shell.h"

/* Storage for baseline times. */
static clock_t start_time;
static struct tms tmbuf_set;

/* Save a baseline of user and system CPU times, plus wallclock time. */
void set_timer(void) {
	/* 取得開始時間 */
	start_time = times(&tmbuf_set);
}


/* Get second set of times, and take delta to get wallclock time.  Display. */
void stop_timer(void) {
	struct tms tmbuf;
	clock_t end_time;
	double ticks;

	/* 取得結束時間 */
	end_time = times(&tmbuf);

	/* Num ticks per sec */
	ticks = sysconf(_SC_CLK_TCK);

	/* Get delta times and print them out. */

	/* system CPU time: 在kernel mode所佔用的CPU時間總和 */
	printf("Sys: %.2f\t", (( tmbuf.tms_cstime - tmbuf_set.tms_cstime ) / ticks ));

	/* user CPU time: 在user mode所佔用的CPU時間總和 */
	printf("User: %.2f\t", (( tmbuf.tms_cutime - tmbuf_set.tms_cutime ) / ticks ));

	/* real time: 開始到結束終止的時間 */
	printf("Real: %.2f\n", ( end_time - start_time ) / ticks );
}

