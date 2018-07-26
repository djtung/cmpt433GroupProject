#include <stdio.h>
#include <stdbool.h>

#include "display.h"
#include "tm.h"

int main(void)
{
  tm_initialize();
  char *digits = "1234";
  while (true) {
    fourDigit_display(digits, true);
  }
}
