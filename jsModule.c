#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#include "timeModule.h"

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"

#define GPIO_IN "in"
#define GPIO_OUT "out"

#define JOY_FILE_UP_DIRECTION "/sys/class/gpio/gpio26/direction"
#define JOY_FILE_RIGHT_DIRECTION "/sys/class/gpio/gpio47/direction"
#define JOY_FILE_DOWN_DIRECTION "/sys/class/gpio/gpio46/direction"
#define JOY_FILE_LEFT_DIRECTION "/sys/class/gpio/gpio65/direction"
#define JOY_FILE_PUSH_DIRECTION "/sys/class/gpio/gpio27/direction"

#define JOY_FILE_UP "/sys/class/gpio/gpio26/value"
#define JOY_FILE_RIGHT "/sys/class/gpio/gpio47/value"
#define JOY_FILE_DOWN "/sys/class/gpio/gpio46/value"
#define JOY_FILE_LEFT "/sys/class/gpio/gpio65/value"
#define JOY_FILE_PUSH "/sys/class/gpio/gpio27/value"

#define JS_FILE_BUFFER_SIZE 5

#define GPIO_JSUP 26
#define GPIO_JSRT 47
#define GPIO_JSDN 46
#define GPIO_JSLFT 65
#define GPIO_JSPSH 27

static int done = 0;
static pthread_t jsTid;

static int JS_Hardware_init();
static void* jsThread(void* arg);
static FILE* openFileWrite(const char* filename);
static FILE* openFileRead(const char* filename);
static int closeFile(FILE* stream);
static int writeGPIO(FILE* file);
static int writeIn(FILE* file);

void JS_startThread() {
	JS_Hardware_init();
	pthread_create(&jsTid, NULL, *jsThread, NULL);
}

void JS_stopThread() {
	done = 1;
	pthread_join(jsTid, NULL);
}

static int JS_Hardware_init() {
	// init joystick GPIO export
	FILE* gpioExportFile = openFileWrite(GPIO_EXPORT_PATH);
	writeGPIO(gpioExportFile);
	closeFile(gpioExportFile);

	// direction: input for all joystick directions
	FILE* upDir = openFileWrite(JOY_FILE_UP_DIRECTION);
	FILE* leftDir = openFileWrite(JOY_FILE_LEFT_DIRECTION);
	FILE* downDir = openFileWrite(JOY_FILE_DOWN_DIRECTION);
	FILE* rightDir = openFileWrite(JOY_FILE_RIGHT_DIRECTION);
	FILE* pushDir = openFileWrite(JOY_FILE_PUSH_DIRECTION);

	writeIn(upDir);
	writeIn(leftDir);
	writeIn(downDir);
	writeIn(rightDir);
	writeIn(pushDir);

	closeFile(upDir);
	closeFile(leftDir);
	closeFile(downDir);
	closeFile(rightDir);
	closeFile(pushDir);

	return 1;
}

// opens a file for writing, returns the pointer to file object
static FILE* openFileWrite(const char* filename) {
	FILE *pFile = fopen(filename, "w");

	if (pFile == NULL) {
		printf("ERROR OPENING %s", filename);
	}

	return pFile;
}

// opens a file for reading, returns the pointer to file object
static FILE* openFileRead(const char* filename) {
	FILE *pFile = fopen(filename, "r");

	if (pFile == NULL) {
		printf("ERROR OPENING %s.", filename);
	}

	return pFile;
}

static int closeFile(FILE* stream) {
	return fclose(stream);
}

// writes GPIO_IN to the specified file
static int writeIn(FILE* file) {
	int charWritten = fprintf(file, GPIO_IN);
	if (charWritten <= 0) {
		printf("ERROR WRITING DATA");
	}

	return charWritten;
}

// for the 4 joystick directions, write them to the export
static int writeGPIO(FILE* file) {
	int charWritten = fprintf(file, "%d", GPIO_JSUP);
	fflush(file);
	if (charWritten <= 0) {
		printf("ERROR WRITING DATA");
		return charWritten;
	}

	charWritten = fprintf(file, "%d", GPIO_JSRT);
	fflush(file);
	if (charWritten <= 0) {
		printf("ERROR WRITING DATA");
		return charWritten;
	}

	charWritten = fprintf(file, "%d", GPIO_JSDN);
	fflush(file);
	if (charWritten <= 0) {
		printf("ERROR WRITING DATA");
		return charWritten;
	}

	charWritten = fprintf(file, "%d", GPIO_JSLFT);
	fflush(file);
	if (charWritten <= 0) {
		printf("ERROR WRITING DATA");
	}

	charWritten = fprintf(file, "%d", GPIO_JSPSH);
	fflush(file);
	if (charWritten <= 0) {
		printf("ERROR WRITING DATA");
	}

	return charWritten;
}

static void* jsThread(void* arg) {
	char bufUp[JS_FILE_BUFFER_SIZE], bufRight[JS_FILE_BUFFER_SIZE];
	char bufDown[JS_FILE_BUFFER_SIZE], bufLeft[JS_FILE_BUFFER_SIZE];
	char bufPush[JS_FILE_BUFFER_SIZE];
	int result, shouldExit;
	FILE* jsUp = NULL;
	FILE* jsRight = NULL;
	FILE* jsDown = NULL;
	FILE* jsLeft = NULL;
	FILE* jsPush = NULL;

	while(!done) {
		result = 0;
		shouldExit = 0;
		// poll for direction press
		while (!done) {
			jsUp = openFileRead(JOY_FILE_UP);
			jsRight = openFileRead(JOY_FILE_RIGHT);
			jsDown = openFileRead(JOY_FILE_DOWN);
			jsLeft = openFileRead(JOY_FILE_LEFT);
			jsPush = openFileRead(JOY_FILE_PUSH);

			fgets(bufUp, 5, jsUp);
			fgets(bufRight, 5, jsRight);
			fgets(bufLeft, 5, jsLeft);
			fgets(bufDown, 5, jsDown);
			fgets(bufPush, 5, jsPush);

			if (!atoi(bufUp)) {
				result = 1;
				break;
			} else if (!atoi(bufRight)) {
				result = 2;
				break;
			} else if (!atoi(bufDown)) {
				result = 3;
				break;
			} else if (!atoi(bufLeft)) {
				result = 4;
				break;
			} else if (!atoi(bufPush)) {
				result = 5;
				break;
			}

			closeFile(jsUp);
			closeFile(jsRight);
			closeFile(jsDown);
			closeFile(jsLeft);
			closeFile(jsPush);
		}

		if (!done) {
			closeFile(jsUp);
			closeFile(jsRight);
			closeFile(jsDown);
			closeFile(jsLeft);
			closeFile(jsPush);
		}

		// wait till direction is de-pressed
		switch(result) {
			case 1:
				do {
					jsUp = openFileRead(JOY_FILE_UP);
					fgets(bufUp, 5, jsUp);
					if (atoi(bufUp)) {
						shouldExit = 1;
					}
					closeFile(jsUp);
				} while (!shouldExit);
				// do something here
				break;
			case 2:
				do {
					jsRight = openFileRead(JOY_FILE_RIGHT);
					fgets(bufRight, 5, jsRight);
					if (atoi(bufRight)) {
						shouldExit = 1;
					}
					closeFile(jsRight);
				} while (!shouldExit);
				// do something here
				break;
			case 3:
				do {
					jsDown = openFileRead(JOY_FILE_DOWN);
					fgets(bufDown, 5, jsDown);
					if (atoi(bufDown)) {
						shouldExit = 1;
					}
					closeFile(jsDown);
				} while (!shouldExit);
				// do something here
				break;
			case 4:
				do {
					jsLeft = openFileRead(JOY_FILE_LEFT);
					fgets(bufLeft, 5, jsLeft);
					if (atoi(bufLeft)) {
						shouldExit = 1;
					}
					closeFile(jsLeft);
				} while (!shouldExit);
				// do something here
				break;
			case 5:
				do {
					jsPush = openFileRead(JOY_FILE_PUSH);
					fgets(bufPush, 5, jsPush);
					if (atoi(bufPush)) {
						shouldExit = 1;
					}
					closeFile(jsPush);
				} while (!shouldExit);
				// TODO: change this
				if(TM_setAlarmStatus(0)) {
					printf("alarm got turned off\n");
				}
				break;
		}
	}
	return NULL;
}