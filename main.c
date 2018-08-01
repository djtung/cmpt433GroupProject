#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "timeModule.h"
#include "audioModule.h"
#include "jsModule.h"
#include "groveDisplay.h"
#include "segDisplay.h"
#include "networkModule.h"

static pthread_mutex_t mainMutex = PTHREAD_MUTEX_INITIALIZER;

int main () {
	AM_init();
	NM_init();
	JS_startThread();
	TM_startThread();
	GROVE_start();
	SEG_start();

	// The program should never stop
	pthread_mutex_lock(&mainMutex);
	pthread_mutex_lock(&mainMutex);

	// This code will not be executed, but is left here in case there is a need
	// for the program to exit gracefully
	/*nanosleep((const struct timespec[]){{3600, 0}}, NULL);

	NM_cleanup();
	TM_stopThread();
	JS_stopThread();
	GROVE_stop();
	SEG_stop();
	AM_cleanup();*/

	return 0;
}