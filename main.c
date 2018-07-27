#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "timeModule.h"
#include "audioModule.h"
#include "jsModule.h"
#include "display.h"

// https://www.thegeekstuff.com/2010/09/change-timezone-in-linux/

/*-------------------------------------------------------------------------

TODO / Notes:
Reference: http://www.cplusplus.com/reference/ctime/

1) Better functionality to interact with 'alarms' file. Might need to
rethink the architecture too... depends on how the user will input / import
new alarms to the clock. (Web UI & Google Calendar). For now, make it
very generic. I think working with UNIX time is still okay

2) Internal alarm cache, periodically saves alarms to file which can be loaded
if the clock turns off (also should update the server when the clock turns on)
Update this cache when a packet is received from the server

3) BBG is ARMv7 which is 32 bits. Meaning this program will be susceptible to
the Year 2038 Problem since we're using 32 bit ints for most time calculation,
but this won't be a problem right? Maybe just mention it during demos :P

4) Lots of error checking for opening files

5) Save to File periodically? (Keep non-volatile memory in case BBG loses power)

6) Smooth out the TTS stuff

---------------------------------------------------------------------------*/

int main () {
	// just some test code for now
	// char buff[100];
	// struct tm testTime;
	// TM_fillStructTM(9, 7, 2018, 21, 40, &testTime);
	// printf("Date 1: %s, unix: %ld\n", asctime(&testTime), mktime(&testTime));

	// printf("%s\n", asctime(&testTime));
	// TM_tttotts(mktime(&testTime), buff);
	// printf("formatted: %s\n", buff);

	// int test, test2, test3;
	// test = TM_getCurrentTime(&test2, &test3);

	// printf("%d %d %d\n", test, test2, test3);

	// AM_init();
	// JS_startThread();
	// nanosleep((const struct timespec[]){{1, 0}}, NULL);
	// TM_startThread();
	DISPLAY_start();

	nanosleep((const struct timespec[]){{300, 0}}, NULL);

	// TM_stopThread();
	// JS_stopThread();
	DISPLAY_stop();
	// AM_cleanup();

	return 0;
}