#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "tm.h"

#define CMD_AUTO_ADDR 0x40
#define START_ADDR 0xc0
#define NUM_DIGITS 4
#define DISPLAY_ON 0x88

#define COLON_FLAG 0x80
#define ASCII_0 48
#define ASCII_9 57

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

static char convertChar(char ch, _Bool colon) {
  char val = 0;
  if ((ASCII_0 <= ch) && (ch <= ASCII_9)) {
    val = displayDigits[ch - ASCII_0];
  }
  if (colon) {
    return val | COLON_FLAG;
  }
  return val;
}

void fourDigit_display(char* digits, _Bool colonOn) {
  assert(strlen(digits) == NUM_DIGITS);

  tm_start();
  tm_write(CMD_AUTO_ADDR);
  tm_stop();

  tm_start();
  tm_write(START_ADDR);
  for (int i = 0; i < NUM_DIGITS; i++) {
    tm_write(convertChar(digits[i], colonOn));
  }
  tm_stop();

  tm_start();
  //This sets it to the brightest
  tm_write(DISPLAY_ON | 0x07);
  tm_stop();
}
