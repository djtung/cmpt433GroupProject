#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "audioModule.h"

#define ALARM_FILE_NAME "test.txt"

static pthread_t tid;
static int done = 0;

static void* TM_Thread(void* arg);
static void bubbleSort(int* array, int length);

void TM_startThread(void) {
	done = 0;
	pthread_create(&tid, NULL, *TM_Thread, NULL);
}

void TM_stopThread(void) {
	done = 1;
	pthread_join(tid, NULL);
}

int* TM_getAlarmsFromFile(int* length) {
	FILE* fp;
	char buffer[100];
	int counter = 0;
	int i = 0;

	// count the number of lines in the file (needed to create the array size)
	fp = fopen(ALARM_FILE_NAME, "r");
	while (fgets(buffer, 100, fp) != NULL) {
		counter++;
	}

	rewind(fp);

	// fill the resulting array (if length != 0)
	if (counter == 0) {
		*length = 0;
		fclose(fp);
		return NULL;
	} else {
		int* newArr = malloc(counter * sizeof(int));

		while(fgets(buffer, 100, fp) != NULL) {
			newArr[i] = atoi(buffer);
			i++;
		}
		*length = i;
		fclose(fp);
		return newArr;
	}
}

int TM_setAlarmsToFile(int* times, int length) {
	FILE* fp;
	fp = fopen(ALARM_FILE_NAME, "w+");

	if (length < 1) {
		// fopen with "w+" will clear the file so we just need to close it
	} else {
		bubbleSort(times, length);

		int i;

		for (i = 0; i < length; i++) {
			fprintf(fp, "%d\n", times[i]);
		}
	}
	
	fclose(fp);
	return 1;
}

int* TM_clearOldAlarms(int* alarms, int* length) {
	int oldLength = *length;
	int newLength = 0;
	int i, now, counter = 0;
	now = time(NULL);

	for (i = 0; i < oldLength; i++) {
		if (alarms[i] > now) {
			newLength++;
		}
	}

	int* newArr = malloc(newLength * sizeof(int));
	for (i = 0; i < oldLength; i++) {
		if (alarms[i] > now) {
			newArr[counter] = alarms[i];
			counter++;
		}
	}

	free(alarms);
	*length = newLength;
	
	return newArr;
}

int TM_clearOldAlarmsInFile() {
	int length;
	int* alarms;
	int* newAlarms;
	alarms = TM_getAlarmsFromFile(&length);
	newAlarms = TM_clearOldAlarms(alarms, &length);
	TM_setAlarmsToFile(newAlarms, length);
	return 1;
}

void TM_itott(int num, time_t* timeStruct) {
	*timeStruct = num;
}

int TM_tmtoi(struct tm* timeptr) {
	return (int) mktime(timeptr);
}

void TM_fillStructTM(int day, int month, int year, int hour, int min, struct tm* newTime) {
	newTime->tm_sec = 0;
	newTime->tm_min = min;
	newTime->tm_hour = hour;
	newTime->tm_mday = day;
	newTime->tm_mon = month-1;
	newTime->tm_year = year-1900;

	// fills in tm_isdst (Daylights savings flag)
	mktime(newTime);
}

static void* TM_Thread(void* arg) {
	time_t now;
	time_t alarm;

	// TODO: we should load this somewhere else so that it can be
	// controlled by the web app or automatically updating
	int length, count = 0;
	int* alarms = TM_getAlarmsFromFile(&length);

	while (!done && count < length) {
		now = time(NULL);
		printf("Time now: %s\n", ctime(&now));

		if (now > alarms[count]) {
			TM_itott(alarms[count], &alarm);
			printf("Alarm %d of %d triggered at %s\n", count, length, ctime(&alarm));
			AM_queueSound(&alarmSound);
			count++;
		}

		// check every 1 second since minutes are our minimum granularity for time
		nanosleep((const struct timespec[]){{1, 0}}, NULL);
	}

	return NULL;
}

// swaps two ints in place
static void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

// sorting time should be negligible enough to use a less than optimal algorithm :)
static void bubbleSort(int* array, int length) {
	int i, j;

	for (i = 0; i < length-1; i++) {
		for (j = 0; j < length-i-1; j++) {
			if (array[j] > array[j+1]) {
				swap(&array[j], &array[j+1]);
			}
		}
	}
}