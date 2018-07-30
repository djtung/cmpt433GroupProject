#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


#define HIGH 1
#define LOW 0
#define IN "in"
#define OUT "out"
#define MAX_BUFFER_LENGTH 1024

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"

#define CLK 2
#define DIO 3
#define CLK_VAL "/sys/class/gpio/gpio2/value"
#define DIO_VAL "/sys/class/gpio/gpio3/value"
#define CLK_DIR "/sys/class/gpio/gpio2/direction"
#define DIO_DIR "/sys/class/gpio/gpio3/direction"

static FILE* openFileWrite(const char* filename);

static void setClk(int value)
{
  FILE *file = fopen(CLK_VAL, "w");
  if (!file) {
    printf("ERROR: Unable to open file (%s) for write\n", CLK_VAL);
    exit(-1);
  }
  if(fprintf(file, "%d", value) < 0) {
    printf("ERROR: Unable to write (%d) into (%s)", value, CLK_VAL);
  }
  fclose(file);
}

static void setDio(int value)
{
  FILE *file = fopen(DIO_VAL, "w");
  if (!file) {
    printf("ERROR: Unable to open file (%s) for write\n", DIO_VAL);
    exit(-1);
  }
  if(fprintf(file, "%d", value) < 0) {
    printf("ERROR: Unable to write (%d) into (%s)", value, DIO_VAL);
  }
  fclose(file);
}

static void wait1(void)
{
  long seconds = 0;
  long nanoseconds = 400;
  struct timespec reqDelay = {seconds, nanoseconds};
  nanosleep(&reqDelay, (struct timespec *) NULL);
}

static void setDirection(int GPIOpin, char *direction)
{
  FILE *file = NULL;
  if (GPIOpin == 2) {
    file = fopen(CLK_DIR, "w");
  }
  else {
    file = fopen(DIO_DIR, "w");
  }
  if (!file) {
    printf("ERROR: Unable to open file GPIO(%d) for write\n", GPIOpin);
    exit(-1);
  }
  if(fprintf(file, "%s", direction) < 0) {
    printf("ERROR: Unable to write (%s) into GPIO(%d)", direction, GPIOpin);
  }
  fclose(file);
}

static int getValue(int GPIOpin)
{
  char value[MAX_BUFFER_LENGTH];

  FILE *file;
  if (GPIOpin == 2) {
    file = fopen(CLK_VAL, "r");
  }
  else {
    file = fopen(DIO_VAL, "r");
  }
  if (!file) {
    printf("ERROR: Unable to open file GPIO(%d) for read\n", GPIOpin);
    exit(-1);
  }
  if(!(fgets(value, MAX_BUFFER_LENGTH, file))) {
    printf("ERROR: Unable to get value from GPIO(%d)", GPIOpin);
  }
  fclose(file);
  return atoi(value);

}

void GH_start(void)
{
  setClk(HIGH);
  setDio(HIGH);
  wait1();

  setDio(LOW);
  wait1();

  setClk(LOW);
  wait1();
}

void GH_stop(void)
{
  setClk(LOW);
  setDio(LOW);
  wait1();

  setClk(HIGH);
  wait1();

  setDio(HIGH);
  wait1();
}

void GH_write(char data)
{
  for(int i = 0; i < 8; i++) {
    setClk(LOW);
    setDio(data & 0x01);
    data >>= 1;
    wait1();

    setClk(HIGH);
    wait1();
  }
  setClk(LOW);
  setDirection(DIO, IN);
  wait1();

  assert(getValue(DIO) == 0);

  setClk(HIGH);
  wait1();

  setClk(LOW);
  setDirection(DIO, OUT);
}

void GH_initializeGroveDisplay(void)
{
  int charWritten = 0;

  FILE* exportFile = openFileWrite(GPIO_EXPORT_PATH);
  charWritten = fprintf(exportFile, "%d", CLK);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
    return;
  }
  fclose(exportFile);

  exportFile = openFileWrite(GPIO_EXPORT_PATH);
  charWritten = fprintf(exportFile, "%d", DIO);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
    return;
  }
  fclose(exportFile);

  setDirection(2, OUT);
  setDirection(3, OUT);
  setClk(1);
  setDio(1);
}


// opens a file for writing, returns the pointer to file object
static FILE* openFileWrite(const char* filename) {
  FILE *pFile = fopen(filename, "w");

  if (pFile == NULL) {
    printf("ERROR OPENING %s", filename);
  }

  return pFile;
}
