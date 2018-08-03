#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#include "groveHelper.h"
#include "timeModule.h"
#include "segDisplay.h"


/******************************************************
 * Grove Display Definitions
 ******************************************************/
#define CMD_AUTO_ADDR 0x40
#define START_ADDR 0xc0
#define NUM_DIGITS 4
#define DISPLAY_ON 0x88

#define COLON_ON 1
#define COLON_FLAG 0x80
#define ASCII_0 48
#define ASCII_9 57

static pthread_t grove_id;
static int loop = 0;

const static char displayDigits[10] = {
  0x3f,
  0x06,
  0x5b,
  0x4f,
  0x66,
  0x6d,
  0x7d,
  0x07,
  0x7f,
  0x67,
};

static char convertChar(char ch, _Bool colon) 
{
  char val = 0;
  if ((ASCII_0 <= ch) && (ch <= ASCII_9)) {
    val = displayDigits[ch - ASCII_0];
  }
  if (colon) {
    return val | COLON_FLAG;
  }
  return val;
}

static void* grove(void* arg) 
{
  GH_initializeGroveDisplay();
  SEG_initializeSegDisplay();
  char digits[4];
  int isPM;
  while(loop){
    isPM = TM_getCurrentTime(digits);
    assert(strlen(digits) == NUM_DIGITS);

    GH_start();
    GH_write(CMD_AUTO_ADDR);
    GH_stop();

    GH_start();
    GH_write(START_ADDR);
    for (int i = 0; i < NUM_DIGITS; i++) {
     GH_write(convertChar(digits[i], COLON_ON));
    }
    GH_stop();

    GH_start();
    GH_write(DISPLAY_ON | 0x07);
    GH_stop();
  }
  GH_deinitializeGroveDisplay();
  SEG_deinitializeSegDisplay();
}

void GROVE_start(void)
{
  loop = 1;
  pthread_create(&grove_id, NULL, *grove, NULL);
}

void GROVE_stop(void)
{
  loop = 0;
  pthread_join(grove_id, NULL);
}