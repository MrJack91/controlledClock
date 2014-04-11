/*
  Simple example to open a maximum of 4 devices - write some data then read it back.
  Shows one method of using list devices also.
  Assumes the devices have a loopback connector on them and they also have a serial number

  To build use the following gcc statement
  (assuming you have the d2xx library in the /usr/local/lib directory).

  build:
  gcc -o dcf77_reader_standalone.out dcf77_reader_standalone.c -L. -lftd2xx
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ftd2xx.h"

#define BUF_SIZE 0x384 // 120 Entries ≈ 2 minutes

#define MAX_DEVICES 5


// http://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
/**
 * Calcs number to string in binary
 * @param x the value to convert in binary
 * @return string binary value
 */
const char *byte_to_binary(int x) {
  static char b[BUF_SIZE];
  b[0] = '\0';

  int z;
  for (z = 128; z > 0; z >>= 1) {
    strcat(b, ((x & z) == z) ? "1" : "0");
  }

  return b;
}

/**
 * prints a buffer structure (data in memory with a pointer)
 * @param buffer buffer pointer
 * @param elements amount of elements in buffer
 * @param type type of output structur (4 ist for combined list)
 */
static void dumpBuffer(unsigned char *buffer, int elements, int type) {
  int j;

  switch (type) {
    case 1:
      // default list coma seperated
      printf(" [");
      for (j = 0; j < elements; j++) {
        if (j > 0)
          printf(", ");
        printf("0x%02X", (unsigned int) buffer[j]);
      }
      printf("]\n");
      break;
    case 2:
      // list with hexa values
      for (j = 0; j < elements; j++) {
        printf("0x%02X\n", (unsigned int) buffer[j]);
      }
      break;
    case 3:
      // list with decimal values
      for (j = 0; j < elements; j++) {
        printf("%3d\n", (unsigned int) buffer[j]);
      }
      break;
    case 4:
      // list with decimal values
      for (j = 0; j < elements; j++) {
        printf("0x%02X\t%3d\n", (unsigned int) buffer[j], (unsigned int) buffer[j]);
      }
      break;
    case 5:
      // list with only dec and coma
      for (j = 0; j < elements; j++) {
        printf("%d,", (unsigned int) buffer[j]);
      }
      printf("\n");
      break;
  }
}

int main() {
  // init vars
  unsigned char * pcBufRead = NULL;
  char * pcBufLD[MAX_DEVICES + 1];
  char cBufLD[MAX_DEVICES][64];
  DWORD dwRxSize = 0, dwRxSizeTemp = 0, dwBytesRead;
  FT_STATUS ftStatus;
  FT_HANDLE ftHandle[MAX_DEVICES];
  int iNumDevs = 0;
  int i;
  int iDevicesOpen = 0;

  // start logic
  printf("start dcf77_reader\n");

  // init mutliple devices access
  for (i = 0; i < MAX_DEVICES; i++) {
    pcBufLD[i] = cBufLD[i];
  }
  pcBufLD[MAX_DEVICES] = NULL;

  // overwrite the correct vector id and product id
  FT_SetVIDPID(1027, 59530); // use our VID and PID
  ftStatus = FT_ListDevices(pcBufLD, &iNumDevs, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);

  if (ftStatus != FT_OK) {
    printf("Error: FT_ListDevices(%d)\n", (int) ftStatus);
    return 1;
  }

  // list founded devices
  for (i = 0; ((i < MAX_DEVICES) && (i < iNumDevs)); i++) {
    printf("Device %d Serial Number - %s\n", i, cBufLD[i]);
  }

  printf("Number of devices: %d.\n", iNumDevs);

  for (i = 0; ((i < MAX_DEVICES) && (i < iNumDevs)); i++) {
    /* Setup */
    if ((ftStatus = FT_OpenEx(cBufLD[i], FT_OPEN_BY_SERIAL_NUMBER, &ftHandle[i])) != FT_OK) {
      /*
        This can fail if the ftdi_sio driver is loaded
        use lsmod to check this and rmmod ftdi_sio to remove
        also rmmod usbserial
       */
      printf("Error FT_OpenEx(%d), device %d\n", (int) ftStatus, i);
      printf("Use lsmod to check if ftdi_sio (and usbserial) are present.\n");
      printf("If so, unload them using rmmod, as they conflict with ftd2xx.\n");
      return 1;
    }

    printf("Opened device %s\n", cBufLD[i]);

    iDevicesOpen++;
    // if ((ftStatus = FT_SetBaudRate(ftHandle[i], 9600)) != FT_OK) {
    if ((ftStatus = FT_SetBaudRate(ftHandle[i], 6000)) != FT_OK) {
      printf("Error FT_SetBaudRate(%d), cBufLD[i] = %s\n", (int) ftStatus, cBufLD[i]);
      break;
    }

    /* Read data from sensor */
    // load buffer
    dwRxSize = 0;
    dwRxSizeTemp = 0;
    printf("Fill Buffer (Size: %d)...\n", BUF_SIZE);
    while ((dwRxSize < BUF_SIZE) && (ftStatus == FT_OK)) {
      ftStatus = FT_GetQueueStatus(ftHandle[i], &dwRxSize);

      // show progress
      if (dwRxSize > dwRxSizeTemp) {
        dwRxSizeTemp = dwRxSize;
        printf("Buffer %d/%d\n", dwRxSize, BUF_SIZE);
      }
    }

    // buffer is filled
    if (ftStatus == FT_OK) {
      // preset buffer
      pcBufRead = realloc(pcBufRead, dwRxSize);
      memset(pcBufRead, 0xFF, dwRxSize);

      ftStatus = FT_Read(ftHandle[i], pcBufRead, dwRxSize, &dwBytesRead);

      if (ftStatus != FT_OK) {
        printf("Error FT_Read(%d)\n", (int) ftStatus);
        break;
      }
      if (dwBytesRead != dwRxSize) {
        printf("FT_Read only read %d (of %d) bytes\n",
                (int) dwBytesRead,
                (int) dwRxSize);
        break;
      }
      
      // output signal for debug
      printf("FT_Read read %d bytes.  Read-buffer is now:\n", (int) dwBytesRead);
      dumpBuffer(pcBufRead, (int) dwBytesRead, 5);
      /*
      printf("list in hex");
      dumpBuffer(pcBufRead, (int) dwBytesRead, 1);

      printf("list in hex");
      dumpBuffer(pcBufRead, (int) dwBytesRead, 2);

      printf("list in dec");
      dumpBuffer(pcBufRead, (int) dwBytesRead, 3);

      printf("list in combined");
      dumpBuffer(pcBufRead, (int) dwBytesRead, 4);
      */
    } else {
      printf("Error FT_GetQueueStatus(%d)\n", (int) ftStatus);
    }
  }

  iDevicesOpen = i;
  /* Cleanup */
  for (i = 0; i < iDevicesOpen; i++) {
    FT_Close(ftHandle[i]);
    printf("Closed device %s\n", cBufLD[i]);
  }

  if (pcBufRead) {
    free(pcBufRead);
  }

  printf("Program end.\n");

  return 0;
}

