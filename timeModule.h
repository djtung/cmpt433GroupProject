// Module for interacting with all things time related.
// Provides functions for reading / writing to local memory,
// spawning threads to check for alarms, and helper functions
// to easily work with C time structures.

// TODO:
// Lots...
// 1) Thread function to check and trigger alarms. Should return a signal
// which can be passed to the controller to trigger an alarm (eg. audio)
// 2) Better functionality to interact with 'alarms' file. Might need to
// rethink the architecture too... depends on how the user will input / import
// new alarms to the clock. (Web UI & Google Calendar). For now, make it
// very generic. I think working with epoch time is still okay

#ifndef _TIME_MODULE_H
#define _TIME_MODULE_H

// Check the file for alarms and load them into memory
// Returns a new array of ints which holds times from epoch of alarms
// 'number' is a pointer to and int which is the length of the array
// The caller should free() the resulting array after use.
int* TM_getAlarmsFromFile(int* number);

// Converts int to time_t
// Fills timeStruct with a integer num which represents time since epoch
void TM_itott(int num, time_t* timeStruct);

// Fills in a time struct easily with parameters
// struct tm reference: www.cplusplus.com/reference/ctime/tm/
void TM_fillStructTM(int day, int month, int year, int hour, int min, struct tm* newTime);

#endif