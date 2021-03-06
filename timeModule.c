#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#include "audioModule.h"

#define ALARM_FILE_NAME "alarmCache.txt"

#define MAX_NUM_ALARMS 50

#define GENERIC_STRING_BUFF_SIZE 100
#define MINIMUM_WAIT_TIME 1

static pthread_t tid;
static pthread_t alarmtid;
static pthread_mutex_t alarmMutex = PTHREAD_MUTEX_INITIALIZER;

static int alarmOn = 0;
static int done = 0;

static int alarmCache[MAX_NUM_ALARMS];
static int alarmCacheLength = 0;
static int currentAlarmIdx = 0;

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
	char buffer[GENERIC_STRING_BUFF_SIZE];
	int counter = 0;
	int i = 0;

	// count the number of lines in the file (needed to create the array size)
	fp = fopen(ALARM_FILE_NAME, "r");
	while (fgets(buffer, GENERIC_STRING_BUFF_SIZE, fp) != NULL) {
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

		while(fgets(buffer, GENERIC_STRING_BUFF_SIZE, fp) != NULL) {
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

	pthread_mutex_lock(&alarmMutex);

	tempArr = TM_clearOldAlarms(arr, &length);
	tempCache = TM_clearOldAlarms(alarmCache, &alarmCacheLength);

	if (tempCache && alarmCacheLength) {
		for (i = 0; i < alarmCacheLength; i++) {
			alarmCache[counter] = tempCache[i];
			counter++;
		}
	}

	if (tempArr && length) {
		for (i = 0; i < length; i++) {
			alarmCache[counter] = tempArr[i];
			counter++;
		}
	}

	bubbleSort(alarmCache, counter);
	alarmCacheLength = counter;
	currentAlarmIdx = 0;

	printTimes(alarmCache, alarmCacheLength);

	TM_setAlarmsToFile(alarmCache, alarmCacheLength);

	pthread_mutex_unlock(&alarmMutex);

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

int TM_getAlarmStatus() {
	return alarmOn;
}

int TM_getCurrentTime(char* result) {
	time_t rawtime = time(NULL);
	struct tm * timeinfo;
	int hour = 0;
	int minute = 0;
	int isPM = 0;

	timeinfo = localtime(&rawtime);

	hour = (*timeinfo).tm_hour;
	minute = (*timeinfo).tm_min;

	// set PM flag
	if (hour >= 12) {
		isPM = 1;
	}

	// get the right hour in 12 hour time
	if (hour == 0) {
		hour = 12;
	} else if (hour > 12) {
		hour-=12;
	}

	// format the string
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

	char date[GENERIC_STRING_BUFF_SIZE];
	char min[GENERIC_STRING_BUFF_SIZE];
	
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
	newTime->tm_hour = hour;
	newTime->tm_mday = day;
	newTime->tm_mon = month-1; // this is just so we can do months 1-12 (more intuitive than 0-11)
	newTime->tm_year = year-1900;

	// fills in tm_isdst (Daylights savings flag)
	mktime(newTime);
}

int TM_getNextAlarm() {
	int result = 0;
	pthread_mutex_lock(&alarmMutex);
	result = alarmCache[0];
	pthread_mutex_unlock(&alarmMutex);
	return result;
}

static void* driverThread(void* arg) {
	time_t now;
	time_t alarm;

	int length = 0;
	int currentAlarm = 0;
	char buff[GENERIC_STRING_BUFF_SIZE];
	char test[GENERIC_STRING_BUFF_SIZE];

	// when the program starts, we should check the memory cache for alarms
	int* alarmsFromFile = TM_getAlarmsFromFile(&length);

	//printTimes(alarmsFromFile, length);

	TM_updateAlarmCache(alarmsFromFile, length);
	free(alarmsFromFile);

	//printf("\n");
	//printTimes(alarmCache, alarmCacheLength);


	while (!done) {
		if (alarmCacheLength && currentAlarmIdx < alarmCacheLength) {
			pthread_mutex_lock(&alarmMutex);
			currentAlarm = alarmCache[currentAlarmIdx];
			pthread_mutex_unlock(&alarmMutex);

			now = time(NULL);
			//printf("Time now: %s\n", ctime(&now));
			TM_getCurrentTime(test);
			//printf("Time now: %s\n", test);
			//printf("Waiting for alarm %d of %d: %d\n", currentAlarmIdx, alarmCacheLength, alarmCache[currentAlarmIdx]);

			if (now > currentAlarm) {
				TM_itott(currentAlarm, &alarm);
				//printf("Alarm %d of %d triggered at %s\n", currentAlarmIdx, alarmCacheLength, ctime(&alarm));

				TM_tttotts(alarm, buff);
				//printf("%s\n", buff);
				AM_playTTS(buff);

				if (startAlarm()) {
					pthread_join(alarmtid, NULL);
					currentAlarmIdx++;
				}
			}

			// check every 1 second since minutes are our minimum granularity for time
			nanosleep((const struct timespec[]){{MINIMUM_WAIT_TIME, 0}}, NULL);
		} else {
			currentAlarmIdx = 0;
		}
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
		// depending on how long the alarm sound is we should pause here to let the sound
		// play out without being interrupted by another sound
		AM_queueSound(&alarmSound);
		nanosleep((const struct timespec[]){{MINIMUM_WAIT_TIME*2, 0}}, NULL);
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
	char result[GENERIC_STRING_BUFF_SIZE];

	for (i = 0; i < length; i++) {
		TM_itott(array[i], &alarm);
		TM_tttotts(alarm, result);
		printf("Arr[%d]: %d, %s\n", i, array[i], result);
	}
}