#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "timeModule.h"

/******************************************************
 * Seg Display Definitions
 ******************************************************/
#define I2CDRV_LINUXBUS1 "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x20
#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

#define export "/sys/class/gpio/export"
#define aPIN 61
#define bPIN 44
#define aPINdir "/sys/class/gpio/gpio61/direction"
#define bPINdir "/sys/class/gpio/gpio44/direction"
#define aPINval "/sys/class/gpio/gpio61/value"
#define bPINval "/sys/class/gpio/gpio44/value"
#define GPIO_OUT "out"

#define CLEAR 0x00
#define A_14 0x91
#define A_15 0x8e
#define P_14 0x11
#define P_15 0x8e
#define M_14 0x81
#define M_15 0xd2

static pthread_mutex_t seg_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_t seg_id;
static int loop = 0;

static FILE* openFileWrite(const char* filename);
static int writeOut(FILE* file);

static int initI2cBus (char* bus, int address)
{
  int i2cFileDesc = open(bus, O_RDWR);
  int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
  return i2cFileDesc;
}

static void SEG_init() {
  int charWritten = 0;

  FILE* exportFile = openFileWrite(export);
  charWritten = fprintf(exportFile, "%d", aPIN);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
    return;
  }
  fclose(exportFile);

  exportFile = openFileWrite(export);
  charWritten = fprintf(exportFile, "%d", bPIN);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
    return;
  }
  fclose(exportFile);

  FILE* directionFile = openFileWrite(aPINdir);
  writeOut(directionFile);
  fclose(directionFile);

  directionFile = openFileWrite(bPINdir);
  writeOut(directionFile);
  fclose(directionFile);
}

static void writeI2cReg (int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
  unsigned char buff[2];
  buff[0] = regAddr;
  buff[1] = value;
  int res = write(i2cFileDesc, buff, 2);
}

static void displayAmPm (int isPM)
{
  FILE *aVAL = fopen(aPINval, "w");
  fprintf(aVAL, "%d", 1);

  int i2cFileDesc = initI2cBus(I2CDRV_LINUXBUS1, I2C_DEVICE_ADDRESS);

  // pthread_mutex_lock(&seg_mtx);
  writeI2cReg(i2cFileDesc, REG_DIRA, CLEAR);
  writeI2cReg(i2cFileDesc, REG_DIRB, CLEAR);

  if(isPM) {
    writeI2cReg(i2cFileDesc, REG_OUTA, P_14);
    writeI2cReg(i2cFileDesc, REG_OUTB, P_15);
  }
  else {
    writeI2cReg(i2cFileDesc, REG_OUTA, A_14);
    writeI2cReg(i2cFileDesc, REG_OUTB, A_15);
  }
  // pthread_mutex_unlock(&seg_mtx);

  fclose(aVAL);
  close(i2cFileDesc);
}

static void displayM (void)
{
  FILE *bbVAL = fopen(bPINval, "w");
  fprintf(bbVAL, "%d", 1);

  int i2cFileDesc = initI2cBus(I2CDRV_LINUXBUS1, I2C_DEVICE_ADDRESS);

  // pthread_mutex_lock(&seg_mtx);
  writeI2cReg(i2cFileDesc, REG_DIRA, CLEAR);
  writeI2cReg(i2cFileDesc, REG_DIRB, CLEAR);

  writeI2cReg(i2cFileDesc, REG_OUTA, M_14);
  writeI2cReg(i2cFileDesc, REG_OUTB, M_15);
  // pthread_mutex_unlock(&seg_mtx);

  fclose(bbVAL);
  close(i2cFileDesc);
}

static void* seg(void* arg) 
{
  char digits[4];
  int isPM;
  FILE *aaVAL;
  FILE *bbVAL;
  while(loop){
  	isPM = TM_getCurrentTime(digits);

    aaVAL = fopen(aPINval, "w");
    bbVAL = fopen(bPINval, "w");
    fprintf(bbVAL, "%d", 0);
    fprintf(aaVAL, "%d", 0);
    fclose(aaVAL);
    fclose(bbVAL);

    displayAmPm(isPM);
    nanosleep((const struct timespec[]){{0, 5000000}}, NULL);

    aaVAL = fopen(aPINval, "w");
    bbVAL = fopen(bPINval, "w");
    fprintf(bbVAL, "%d", 0);
    fprintf(aaVAL, "%d", 0);
    fclose(aaVAL);
    fclose(bbVAL);

    displayM();
    nanosleep((const struct timespec[]){{0, 5000000}}, NULL);
  }
}

void SEG_start ()
{
  SEG_init();
  loop = 1;
  pthread_create(&seg_id, NULL, *seg, NULL);
}

void SEG_stop ()
{
  loop = 0;
  pthread_join(seg_id, NULL);
}

// opens a file for writing, returns the pointer to file object
static FILE* openFileWrite(const char* filename) {
  FILE *pFile = fopen(filename, "w");

  if (pFile == NULL) {
    printf("ERROR OPENING %s", filename);
  }

  return pFile;
}

// writes GPIO_OUT to the specified file
static int writeOut(FILE* file) {
  int charWritten = fprintf(file, GPIO_OUT);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
  }

  return charWritten;
}

void SEG_initializeSegDisplay(void)
{
   int charWritten = 0;

  FILE* exportFile = openFileWrite(aPINdir);
  charWritten = fprintf(exportFile, "%s", OUT);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
    return;
  }
  fclose(exportFile);

  exportFile = openFileWrite(bPINdir);
  charWritten = fprintf(exportFile, "%s", OUT);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
    return;
  }
  fclose(exportFile);

  FILE* exportFile = openFileWrite(aPINval);
  charWritten = fprintf(exportFile, "%d", 1);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
    return;
  }
  fclose(exportFile);

  exportFile = openFileWrite(bPINdir);
  charWritten = fprintf(exportFile, "%d", 1);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
    return;
  }
  fclose(exportFile);
}

void SEG_deinitializeSegDisplay(void)
{
  int charWritten = 0;

  FILE* exportFile = openFileWrite(aPINval);
  charWritten = fprintf(exportFile, "%d", 0);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
    return;
  }
  fclose(exportFile);

  exportFile = openFileWrite(bPINdir);
  charWritten = fprintf(exportFile, "%d", 0);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
    return;
  }

  FILE* exportFile = openFileWrite(aPINdir);
  charWritten = fprintf(exportFile, "%s", IN);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
    return;
  }
  fclose(exportFile);

  exportFile = openFileWrite(bPINdir);
  charWritten = fprintf(exportFile, "%s", IN);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
    return;
  }
  fclose(exportFile);
}