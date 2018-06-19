#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

int* TM_getAlarmsFromFile(int* number) {
	FILE *fp;
	char buffer[100];
	int counter = 0;
	int i = 0;

	fp = fopen("test.txt", "r");
	while (fgets(buffer, 100, fp) != NULL) {
		counter++;
	}

	rewind(fp);

	if (counter == 0) {
		*number = 0;
		fclose(fp);
		return NULL;
	} else {
		int* newArr = malloc(counter * sizeof(int));

		while(fgets(buffer, 100, fp) != NULL) {
			newArr[i] = atoi(buffer);
			i++;
		}
		*number = i;
		fclose(fp);
		return newArr;
	}
}

void TM_itott(int num, time_t* timeStruct) {
	*timeStruct = num;
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