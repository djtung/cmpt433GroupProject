// jsModule.h

// Module to provide interaction with the joystick on the Zen Cape via GPIO

#ifndef JS_MODULE_H
#define JS_MODULE_H

// Begin/end the background thread which starts the listeners
// for the joystick
void JS_startThread();
void JS_stopThread();

#endif