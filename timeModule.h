// Module for interacting with all things time related.
// Provides functions for reading / writing to local memory,
// spawning threads to check for alarms, and helper functions
// to easily work with C time structures.

#ifndef TIME_MODULE_H
#define TIME_MODULE_H

// Start and stop the checker thread for alarms in the file
void TM_startThread(void);
void TM_stopThread(void);

// Check the file for alarms and load them into memory
// Returns a new array of ints which holds alarms in UNIX time
// 'length' is a pointer to an int which stores the length of the array
// The caller should free() the resulting array after use.
int* TM_getAlarmsFromFile(int* length);

// Writes an array of alarms to the file (non-volatile memory)
// Returns a '1' on success or '0' on failure to write
// 'times' is an array of ints which are alarms in UNIX time
// 'length' is the length of the array. A 0 for length clears the file
//
// TM_setAlarmsToFile will order the alarms such that the earliest alarms will appear before later alarms
int TM_setAlarmsToFile(int* times, int length);

// Takes an array of alarms in UNIX time and purges alarms that have already passed
// 'alarms' is an array of alarms, which are UNIX time integers
// 'length' is a pointer to an integer which is the length of 'alarms'
//
// returns a new array with the old alarms purged
// 'length' rewritten to store the new length of the new array
int* TM_clearOldAlarms(int* alarms, int* length);

// Removes old alarms in the internal memory
// returns a 1 on success or 0 on failure
int TM_clearOldAlarmsInFile();

// Sets the status of the alarm to On (1) or Off (0)
// returns 1 on successful set
int TM_setAlarmStatus(int status);

// Gets the current time (possibly for the display)
// 'result' gets stored with a 4 digits of the time (HourMinute)
// return 0 for AM, 1 for PM
int TM_getCurrentTime(char* result);

// Converts int (UNIX time) to time_t
// Fills timeStruct with a 'num' which is a UNIX time integer
void TM_itott(int num, time_t* timeStruct);

// Converts a struct tm to integer (UNIX time)
int TM_tmtoi(struct tm* timeptr);

// Converts a time_t to a Time and Date for TTS
void TM_tttotts(time_t unixTime, char* result);

// Fills in a time struct easily with parameters
// struct tm reference: www.cplusplus.com/reference/ctime/tm/
void TM_fillStructTM(int day, int month, int year, int hour, int min, struct tm* newTime);

// HELPFUL FUNCTIONS FROM TIME.H:

// convert time_t to string (local time)
// char* ctime (const time_t * timer);

// convert struct tm to time_t
// time_t mktime (struct tm* timeptr);

// formats struct tm* as string
// size_t strftime (char* ptr, size_t maxsize, const char* format, const struct tm* timeptr );
#endif