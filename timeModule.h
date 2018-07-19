// Module for interacting with all things time related.
// Provides functions for reading / writing to local memory,
// spawning threads to check for alarms, and helper functions
// to easily work with C time structures.

// TODO / Notes:
// Reference: http://www.cplusplus.com/reference/ctime/

// 2) Better functionality to interact with 'alarms' file. Might need to
// rethink the architecture too... depends on how the user will input / import
// new alarms to the clock. (Web UI & Google Calendar). For now, make it
// very generic. I think working with UNIX time is still okay

// 3) Stop Alarm thread from joystick and get/set alarmOn (global status var)
//
//
// 4) BBG is ARMv7 which is 32 bits. Meaning this program will be susceptible to
// the Year 2038 Problem since we're using 32 bit ints for most time calculation,
// but this won't be a problem right? Maybe just mention it during demos :P
//
// 5) Lots of error checking for opening files

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

// Gets the current time (possibly for the display)
// 'hour' is filled with hours since midnight (0-23)
// 'min' is filled with minutes after the hour (0-59)
// return 0 for AM, 1 for PM
int TM_getCurrentTime(int* hour, int* minute);

// Converts int (UNIX time) to time_t
// Fills timeStruct with a 'num' which is a UNIX time integer
void TM_itott(int num, time_t* timeStruct);

// Converts a struct tm to integer (UNIX time)
int TM_tmtoi(time_t* timeStruct);

// Fills in a time struct easily with parameters
// struct tm reference: www.cplusplus.com/reference/ctime/tm/
void TM_fillStructTM(int day, int month, int year, int hour, int min, struct tm* newTime);

// HELPFUL FUNCTIONS FROM TIME.H:

// convert time_t to string (local time)
// char* ctime (const time_t * timer);

// convert struct tm to time_t
// time_t mktime (struct tm* timeptr);
#endif