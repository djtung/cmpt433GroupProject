#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "timeModule.h"
#include "audioModule.h"

int main () {
	// just some test code for now
	struct tm testTime;
	TM_fillStructTM(9, 7, 2018, 21, 40, &testTime);
	printf("Date 1: %s, unix: %ld\n", asctime(&testTime), mktime(&testTime));

	AM_init();
	nanosleep((const struct timespec[]){{1, 0}}, NULL);
	TM_startThread();
	nanosleep((const struct timespec[]){{300, 0}}, NULL);
	TM_stopThread();
	AM_cleanup();

	return 0;
}