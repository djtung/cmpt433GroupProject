#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "timeModule.h"
#include "audioModule.h"
#include "jsModule.h"

// https://www.thegeekstuff.com/2010/09/change-timezone-in-linux/

int main () {
	// just some test code for now
	struct tm testTime;
	TM_fillStructTM(9, 7, 2018, 21, 40, &testTime);
	printf("Date 1: %s, unix: %ld\n", asctime(&testTime), mktime(&testTime));

	int test, test2, test3;
	test = TM_getCurrentTime(&test2, &test3);

	printf("%d %d %d\n", test, test2, test3);

	AM_init();
	nanosleep((const struct timespec[]){{1, 0}}, NULL);
	TM_startThread();
	nanosleep((const struct timespec[]){{300, 0}}, NULL);
	TM_stopThread();
	AM_cleanup();

	return 0;
}