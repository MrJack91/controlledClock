/*
 * read the signal from the sensor
 build:
 clear && rm -f dcf77_decoder && gcc -o dcf77_decoder dcf77_decoder.c && ./dcf77_decoder
 * 
 * todo:
 *  - build the whole project (go make cross-file-function-calls)
 *  - use list instead of array for data
 * 
 * links:
 *  - http://blog.blinkenlight.net/2012/12/01/dcf77-project/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// max count of element in buffer
#define BUFFER_ELEMENTS 100

// current received data (relative number bit-20) - buffer/cache for 100 numbers
unsigned int data[BUFFER_ELEMENTS];

// calc buffersize for 100 ints
int BUFFER_SIZE = BUFFER_ELEMENTS* sizeof(unsigned int);

int currentPointer = 0;

void resetData() {
  currentPointer = 0;
}

/**
 * calcs the relative bit position (bitpos - 20)
 * INFO: only use them for direct access to data[]
 * @return 
 */
int getRelativeBitPos(int bitPos) {
  return bitPos - 20;
}

void printData() {
  for (int i = 0; i < currentPointer; i++) {
    printf("%d ", data[i]);
  }
  printf("\n");
}

/**
 * check the parity from a range in data[]
 * defintion of even parity: sum of all 1's must be even
 * @param from range info: from index
 * @param to range info: to index (to must be the parity/check bit)
 * @return result 1 means parity was successful
 */
int checkParity(int from , int to) {
  int result = 1; // at the end result must be 1 again
  // printf("check parity from %d to %d\n", from, to);
  for (int i = from; i <= to; i++) {
    // toggle temporary result by reading 1
    // printf("%d\n", data[getRelativeBitPos(i)]);
    if (data[getRelativeBitPos(i)] == 1) {
      result = abs(result-1);
    }
    // printf("temporary result of Index %d is: %d\n", i, result);
  }
  return result;
}

/**
 * try to read a valid date from the data
 * @param recursive should by try to call it self for error resolving
 */
int tryToReadDate(int currentPointer, int recursive) {
  // printf("try to read date from data\n");
  
  // int parityCheck = checkParity(20, 20+currentPointer);
  // printf("parityCheck is: %d\n", parityCheck);
  
  int parityCheck = 1;  // init with valid data
  int tryAgaing = 1;    // if he should try again to find a solution
  
  // first bit [bit 20] must be 1 -> start of time
  if (data[getRelativeBitPos(20)] == 1) {
    // select a parity to check - take care: values are absolute, because c want to have int in a switch condition
    switch (currentPointer) {
      case 8: // check minute value
        parityCheck = checkParity(20, 28);
        break;
      case 15: // check hour value
        parityCheck = checkParity(29, 35);
        break;
      case 38: // check date value
        parityCheck = checkParity(36, 58);
        break;
    }
  } else {
    parityCheck = 0;
  }
    
  // if there a problem with the parity, try by the next value
  if (parityCheck == 0) {
    printf("ou.. parity goes wrong - fix it (recursion: %d)\n", recursive);
    if (recursive == 1) {
      tryAgaing = 1;
      while (tryAgaing == 1) {
        // remove first value
        printf("%p\n", data);
        printf("%p\n", data+sizeof(int));
        printData();
        memmove(&data[0], &data[1], (BUFFER_SIZE-1) * sizeof(unsigned int));
        printData();
        data[BUFFER_ELEMENTS-1] = 0;
        currentPointer--;

        // after re shift data, valid them again
        int valuesToCheck[] = { 1, 28, 35, 58 };
        for (int i = 0; i <= 3; i++) {
          // only check value if there is anything in it
          if (valuesToCheck[i] <= currentPointer) {
            tryAgaing = tryToReadDate(valuesToCheck[i], 0);
          }
        }
      }
    }
  } else {
    tryAgaing = 0;
  }
  
  return tryAgaing;
  
  
  // [0] start of minute
  // [1-14] stuff (infos)
  // [15-19] important time data
  
  // [20] [0] get first 1 -> start bit
  
  // [28] [8]check bit minute
  
  // [35] [15] check bit hour
  
  // [36-41] day
  
  // [42-44] weekday
  
  // [45-49] month
  
  // [50-57] year
  
  // [58] check bit date (date-year)
  
  // 59 empty
  
  
  
}

void addReceivedByte(unsigned int received) {
  int bit = 1;
  // magic mapping: http://wiki.gude.info/FAQ_mouseCLOCK_Programmierung
  if (received >= 192) {
    bit = 0;
  }
  data[currentPointer] = bit;
  tryToReadDate(currentPointer, 1);
  currentPointer++;
}

int main() {
  printf("start dcf77_decoder\n");
  
  printf("%p\n", data);
  
  // 120 predefined values (timestamp near: 23:10:50 DI 08.04.2014)) - by bit ≈43
  int temporaryData[] = { 252,252,252,0,252,252,252,0,252,252,252,128,252,252,252,252,128,252,128,252,252,252,0,252,254,252,128,0,128,128,128,252,128,252,252,252,254,128,252,254,0,254,254,128,252,252,252,252,128,252,252,128,0,128,252,252,252,128,128,252,254,254,0,252,252,254,0,254,252,252,128,252,252,252,252,128,252,0,254,254,254,128,254,0,252,0,128,128,252,252,252,252,252,254,0,254,128,254,252,128,252,252,128,128,252,254,252,128,252,252,252,0,128,252,254,252,128,128,252,252 };
  int countValues = sizeof(temporaryData) / sizeof(*temporaryData);
  printf("sizeof(temporaryData): %u\n", countValues);
  for (int i = 0; i <= countValues; i++) {
    addReceivedByte(temporaryData[i]);
  }
  
  printf("print data:\n");
  printData();
  
  return 0;
}