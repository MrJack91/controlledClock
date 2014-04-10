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

int debug_readPos = 0;

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
  // printf("print data from %d to %d\n", 0, currentPointer);
  for (int i = 0; i < currentPointer; i++) {
    printf("%d ", data[i]);
  }
  printf(" - (length: %d)\n", currentPointer);
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
  // printf("******************************************\nresult of parity check: %d\n******************************************\n", result);
  return result;
}

/**
 * try to read a valid date from the data
 * @param recursive should by try to call it self for error resolving
 * @return success decode (1 = succesfull) -  if it call it recursivle it will be always true
 */
int tryToReadDate(int recursive) {
  // printf("try to read date from data\n");
  
  /*
  if (recursive == 0) {
    // you're in recursion print it readable
    printf("-- ");
  }
  printf("currentPointer current %d\n", currentPointer);
  printData();
  */
  
  int parityCheck = 1;  // init with valid data
  // int tryAgain = 0;     // if he should try again to find a solution
  int foundValidDate = 0;
  int success = 0;
  
  // first bit [bit 20] must be 1 -> start of time
  if (data[getRelativeBitPos(20)] == 1) {
    // select a parity to check - take care: values are absolute, because c want to have int in a switch condition
    switch (currentPointer-1) {
      case 8: // check minute value
        parityCheck = checkParity(21, 28);
        break;
      case 15: // check hour value
        parityCheck = checkParity(29, 35);
        break;
      case 38: // check date value
        parityCheck = checkParity(36, 58);
        if (parityCheck == 1) {
          foundValidDate = 1;
        }        
        break;
    }
  } else {
    // at first there must be a 1
    parityCheck = 0;
  }
  
  if (foundValidDate == 1) {
    // found a valid data
    printf("***************************************************************************\n"
            "YEAH: found a valid date\n"
            "***************************************************************************\n");
    printData();
  }
  
  // if there a problem with the parity, try by the next value
  if (parityCheck == 0) {
    // printf("ou.. parity goes wrong - fix it (recursion: %d)\n", recursive);
    // printf("print data:\n");
    // printData();
    if (recursive == 1) {
      int tryAgain = 1;
      while (tryAgain == 1) {
        // remove first value
        // printf("%p\n", &data[0]);
        // printf("%p\n", &data[1]);
        // printData();
        
        if (currentPointer > 0) {
        
          // move first element out
          // memmove(data, data+1, (BUFFER_ELEMENTS-1)*sizeof(*data));
          memmove(&data[0], &data[1], (BUFFER_ELEMENTS-1)*sizeof(*data));
          // memmove(&data[0], &data[1], (BUFFER_SIZE-1) * sizeof(unsigned int));
          data[BUFFER_ELEMENTS-1] = 0;

          // check abort condition
          // if (currentPointer > 0) {
        
          // printf("currentPointer before %d\n", currentPointer);
          currentPointer--;

          // printf("currentPointer after %d\n", currentPointer);
          // printData();
        
          // after re shift data, valid them again
          int tempCurrentPointer = currentPointer;
          // int valuesToCheck[] = { 20, 28, 35, 58 };
          int valuesToCheck[] = { 0, 8, 15, 38 };
          for (int i = 0; i <= 3; i++) {
            // only check value if there is anything in it
            if (valuesToCheck[i] <= tempCurrentPointer) {
              currentPointer = valuesToCheck[i];
              // printf("parity check for pointer: %d\n", currentPointer);
              int tempSuccess = tryToReadDate(0);
              // printf("state of tryAgaing: %d\n", tryAgain);
              if (tempSuccess == 0) {
                // there was a parity error -> break and try again with next bit
                tryAgain = 1;
                break; // leave check parity for this sequence
              }
            }
          }
          // reset the pointer to origin data position
          currentPointer = tempCurrentPointer;
        } else {
          // if data array is now empty, leave check
          tryAgain = 0;
          success = 1;
        }
      } // loop for search a valid part in the datastream
    } else {
      // no recursion - give last parity back
      success = parityCheck;
    }
  } else {
    // tryAgain = 0;
    success = 1;
  }
  
  // return tryAgain;
  return success;
}

void addReceivedByte(unsigned int received) {
  // printf("\n\ndebug_readPos: %d\n", debug_readPos);
  // printData();
  debug_readPos++;
  int bit = 1;
  // magic mapping: http://wiki.gude.info/FAQ_mouseCLOCK_Programmierung
  if (received >= 192) {
    bit = 0;
  }
  
  data[currentPointer] = bit;
  currentPointer++;
  tryToReadDate(1);
}

int main() {
  printf("start dcf77_decoder\n");
  
  // printf("%p\n", data);
  
  // 120 predefined values (timestamp near: 23:10:50 DI 08.04.2014)) - by bit ≈43
  // full 2 mintues
  int temporaryData[] = { 252,252,252,0,252,252,252,0,252,252,252,128,252,252,252,252,128,252,128,252,252,252,0,252,254,252,128,0,128,128,128,252,128,252,252,252,254,128,252,254,0,254,254,128,252,252,252,252,128,252,252,128,0,128,252,252,252,128,128,252,254,254,0,252,252,254,0,254,252,252,128,252,252,252,252,128,252,0,254,254,254,128,254,0,252,0,128,128,252,252,252,252,252,254,0,254,128,254,252,128,252,252,128,128,252,254,252,128,252,252,252,0,128,252,254,252,128,128,252,252 };
  // only valid data
  // int temporaryData[] = { 128,252,252,252,252,128,252,252,128,0,128,252,252,252,128,128,252,254,254,0,252,252,254,0,254,252,252,128,252,252,252,252,128,252,0,254,254,254,128,254 };
  // only the minutes
  // int temporaryData[] = { 128,252,252,252,252,128,252,252,128 };
  // only the minutes and hours
  // int temporaryData[] = { 128,252,252,252,252,128,252,252,128, 0,128,252,252,252,128,128 };
  int countValues = sizeof(temporaryData) / sizeof(*temporaryData);
  printf("sizeof(temporaryData): %u\n", countValues);
  for (int i = 0; i <= countValues; i++) {
    addReceivedByte(temporaryData[i]);
  }
  
  return 0;
}