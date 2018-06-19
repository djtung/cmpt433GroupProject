#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "timeModule.h"

int main () {
	// just some test code for now
	struct tm testTime, testTime2, testTime3;

	TM_fillStructTM(18, 6, 2018, 20, 0, &testTime);
	TM_fillStructTM(20, 6, 2018, 8, 0, &testTime2);
	TM_fillStructTM(22, 6, 2018, 8, 0, &testTime3);

	printf("Date 1: %s", asctime(&testTime));
	printf("Date 2: %s", asctime(&testTime2));
	printf("Date 3: %s", asctime(&testTime3));

	int* testArr;
	int num = 0;

	testArr = TM_getAlarmsFromFile(&num);

	for (int i = 0; i < num; i++) {
		printf("%d\n", testArr[i]);
	}

	printf("num: %d\n", num);

	time_t temp;
	TM_itott(testArr[0], &temp);

	printf("%s\n", ctime(&temp));
	return 0;
}