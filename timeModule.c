#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#include "audioModule.h"

#define ALARM_FILE_NAME "test.txt"

#define MAX_NUM_ALARMS 50

static pthread_t tid;
static pthread_t alarmtid;
static pthread_mutex_t alarmMutex = PTHREAD_MUTEX_INITIALIZER;

static int alarmOn = 0;
static int done = 0;

static int alarmCache[MAX_NUM_ALARMS];
static int alarmCacheLength = 0;

static void* driverThread(void* arg);
static void* alarmThread(void* arg);
static void bubbleSort(int* array, int length);
static void printTimes(int* array, int length);
static int startAlarm();

void TM_startThread(void) {
	done = 0;
	pthread_create(&tid, NULL, *driverThread, NULL);
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

	if (newLength) {
		int* newArr = malloc(newLength * sizeof(int));
		for (i = 0; i < oldLength; i++) {
			if (alarms[i] > now) {
				newArr[counter] = alarms[i];
				counter++;
			}
		}

		*length = newLength;
		
		return newArr;
	} else {
		return NULL;
	}

}

int TM_clearOldAlarmsInFile() {
	int length = 0;
	int* alarms;
	int* newAlarms;
	alarms = TM_getAlarmsFromFile(&length);
	newAlarms = TM_clearOldAlarms(alarms, &length);
	TM_setAlarmsToFile(newAlarms, length);

	if (alarms) {
		free(alarms);
	}
	if (newAlarms) {
		free(newAlarms);
	}

	return 1;
}

void TM_updateAlarmCache(int* arr, int length) {
	int i, counter = 0;
	int* tempArr;
	int* tempCache;

	tempArr = TM_clearOldAlarms(arr, &length);
	tempCache = TM_clearOldAlarms(alarmCache, &alarmCacheLength);

	if (tempArr && length) {
		for (i = 0; i < length; i++) {
			alarmCache[counter] = tempArr[i];
			counter++;
		}
	}
	if (tempCache && alarmCacheLength) {
		for (i = 0; i < alarmCacheLength; i++) {
			alarmCache[counter] = tempCache[i];
			counter++;
		}
	}

	bubbleSort(alarmCache, counter);
	alarmCacheLength = counter;

	if (tempArr) {
		free(tempArr);
	}
	if (tempCache) {
		free(tempCache);
	}
}

int TM_setAlarmStatus(int status) {
	alarmOn = status;
	return 1;
}

int TM_getCurrentTime(char* result) {
	time_t rawtime = time(NULL);
	struct tm * timeinfo;
	int hour, minute;
	int isPM = 0;

	timeinfo = localtime(&rawtime);

	hour = (*timeinfo).tm_hour;
	minute = (*timeinfo).tm_min;

	if (hour >= 12) {
		isPM = 1;
	}
	if (hour > 12) {
		hour-=12;
	}

	if (hour < 10) {
		if (minute < 10) {
			sprintf(result, "0%d0%d", hour, minute);
		} else {
			sprintf(result, "0%d%d", hour, minute);
		}
	} else {
		if (minute < 10) {
			sprintf(result, "%d0%d", hour, minute);
		} else {
			sprintf(result, "%d%d", hour, minute);
		}
	}

	if (isPM) {
		return 1;
	} else {
		return 0;
	}
}

void TM_itott(int num, time_t* timeStruct) {
	*timeStruct = num;
}

int TM_tmtoi(struct tm* timeptr) {
	return (int) mktime(timeptr);
}

void TM_tttotts(time_t unixTime, char* result) {
	struct tm * timeinfo;
	timeinfo = localtime(&unixTime);

	char date[50];
	char min[50];
	
	int hour = 0;

	strftime(date, 50, "%A %B %d. ", timeinfo);

	hour = timeinfo->tm_hour;

	int actualHour = hour;

	if (hour == 0) {
		actualHour = 12;
	} else if (hour > 12) {
		actualHour = hour % 12;
	}

	if (timeinfo->tm_min == 0) {
		strftime(min, 50, "%p", timeinfo);
	} else if (timeinfo->tm_min < 10) {
		strftime(min, 50, ". o %M %p", timeinfo);
	} else {
		strftime(min, 50, ". %M %p", timeinfo);
	}

	sprintf(result, "%s %d%s", date, actualHour, min);
}

void TM_fillStructTM(int day, int month, int year, int hour, int min, struct tm* newTime) {
	newTime->tm_sec = 0;
	newTime->tm_min = min;
	newTime->tm_hour = hour-1; // don't know why it increments hour by 1, so we have to -1 here
	newTime->tm_mday = day;
	newTime->tm_mon = month-1; // this is just so we can do months 1-12 (more intuitive than 0-11)
	newTime->tm_year = year-1900;

	// fills in tm_isdst (Daylights savings flag)
	mktime(newTime);
}

static void* driverThread(void* arg) {
	time_t now;
	time_t alarm;

	int length, count = 0;
	int currentAlarm = 0;
	char buff[100];

	// Get alarms from Web (Might be from UDP module)
	//int* temp2 = TM_getAlarmsFromWeb(&length2);
	//int* alarmsFromWeb = TM_clearOldAlarms(temp2, &length2);

	/*int* temp = TM_getAlarmsFromFile(&length);
	int* alarmsFromFile = TM_clearOldAlarms(temp, &length);*/
	int* alarmsFromFile = TM_getAlarmsFromFile(&length);

	printTimes(alarmsFromFile, length);

	pthread_mutex_lock(&alarmMutex);
	TM_updateAlarmCache(alarmsFromFile, length);
	pthread_mutex_unlock(&alarmMutex);

	printf("\n");
	printTimes(alarmCache, alarmCacheLength);

	free(alarmsFromFile);

	while (!done && count < alarmCacheLength) {
		pthread_mutex_lock(&alarmMutex);
		currentAlarm = alarmCache[count];
		pthread_mutex_unlock(&alarmMutex);

		now = time(NULL);
		printf("Time now: %s\n", ctime(&now));

		if (now > currentAlarm) {
			TM_itott(currentAlarm, &alarm);
			printf("Alarm %d of %d triggered at %s\n", count, length, ctime(&alarm));

			TM_tttotts(alarm, buff);
			printf("%s\n", buff);
			AM_playTTS(buff);

			if (startAlarm()) {
				pthread_join(alarmtid, NULL);
				count++;
			}
		}

		// check every 1 second since minutes are our minimum granularity for time
		nanosleep((const struct timespec[]){{1, 0}}, NULL);
	}

	return NULL;
}

// returns 1 for alarm got started
// returns 0 for alarm failed to start
static int startAlarm() {
	if (!alarmOn) {
		TM_setAlarmStatus(1);
		pthread_create(&alarmtid, NULL, *alarmThread, NULL);
		return 1;
	}

	return 0;
}

static void* alarmThread(void* arg) {
	while (alarmOn) {
		AM_queueSound(&alarmSound);
		nanosleep((const struct timespec[]){{5, 0}}, NULL);
	}
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

// for debugging
static void printTimes(int* array, int length) {
	int i;
	time_t alarm;
	char result[100];

	for (i = 0; i < length; i++) {
		TM_itott(array[i], &alarm);
		TM_tttotts(alarm, result);
		printf("Arr[%d]: %d, %s\n", i, array[i], result);
	}
}