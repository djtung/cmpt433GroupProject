#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "tm.h"
#include "timeModule.h"


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
#define aPINdir "/sys/class/gpio/gpio61/direction"
#define bPINdir "/sys/class/gpio/gpio44/direction"
#define aPINval "/sys/class/gpio/gpio61/value"
#define bPINval "/sys/class/gpio/gpio44/value"
#define CLEAR 0x00
#define A_14 0x91
#define A_15 0x8e
#define P_14 0x11
#define P_15 0x8e
#define M_14 0x81
#define M_15 0xd2


// static pthread_mutex_t display_mtx = PTHREAD_MUTEX_INTIALIZER;
static pthread_t display_id;
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

static int initI2cBus (char* bus, int address)
{
  int i2cFileDesc = open(bus, O_RDWR);
  int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
  return i2cFileDesc;
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
  fclose(aVAL);
  FILE *bVAL = fopen(bPINval, "w");
  fprintf(bVAL, "%d", 0);
  fclose(bVAL);

  int i2cFileDesc = initI2cBus(I2CDRV_LINUXBUS1, I2C_DEVICE_ADDRESS);

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

  close(i2cFileDesc);
}

static void displayM (void)
{
  FILE *aaVAL = fopen(aPINval, "w");
  fprintf(aaVAL, "%d", 0);
  fclose(aaVAL);
  FILE *bbVAL = fopen(bPINval, "w");
  fprintf(bbVAL, "%d", 1);
  fclose(bbVAL);

  int i2cFileDesc = initI2cBus(I2CDRV_LINUXBUS1, I2C_DEVICE_ADDRESS);

  writeI2cReg(i2cFileDesc, REG_DIRA, CLEAR);
  writeI2cReg(i2cFileDesc, REG_DIRB, CLEAR);

  writeI2cReg(i2cFileDesc, REG_OUTA, M_14);
  writeI2cReg(i2cFileDesc, REG_OUTB, M_15);

  close(i2cFileDesc);
}

static void* display(void* arg) 
{
  tm_initializeGroveDisplay();
  struct timespec reqDelay = { (long)0, (long)5000000 }; 
  char* digits;
  while(loop){
    int isPM = TM_getCurrentTime(digits);

    assert(strlen(digits) == NUM_DIGITS);

    tm_start();
    tm_write(CMD_AUTO_ADDR);
    tm_stop();

    tm_start();
    tm_write(START_ADDR);
    for (int i = 0; i < NUM_DIGITS; i++) {
     tm_write(convertChar(digits[i], COLON_ON));
    }
    tm_stop();

    tm_start();
    //This sets it to the brightest
    tm_write(DISPLAY_ON | 0x07);
    tm_stop();

    nanosleep(&reqDelay, (struct timespec *) NULL);
    displayAmPm(isPM);
    nanosleep(&reqDelay, (struct timespec *) NULL);
    displayM();
  }
}

void DISPLAY_start(void)
{
  loop = 1;
  pthread_create(&display_id, NULL, *display, NULL);
}

void DISPLAY_stop(void)
{
  loop = 0;
  pthread_join(display_id, NULL);
}