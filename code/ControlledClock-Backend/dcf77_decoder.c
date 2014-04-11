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
#include <math.h> // to use the pow command

// max count of element in buffer
#define BUFFER_ELEMENTS 100

// current received data (relative number bit-20) - buffer/cache for 100 numbers
unsigned int data[BUFFER_ELEMENTS];

// calc buffersize for 100 ints
int BUFFER_SIZE = BUFFER_ELEMENTS* sizeof(unsigned int);

int currentPointer = 0;

// int debug_readPos = 0;

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
  for (int i = from; i <= to; i++) {
    // toggle temporary result by reading 1
    if (data[getRelativeBitPos(i)] == 1) {
      result = abs(result-1);
    }
  }
  return result;
}

/**
 * calc a number with value 1,2,4,8,10,20,40,80
 * @param from
 * @param to
 * @return number of value
 */
int readDCFNumber(int from, int to) {
  // printf("from: %d; to: %d\n", from, to);
  int result = 0;
  int relPos = 0;
  int val = 0;
  int calc = 0;
  for (int i = from; i <= to; i++) {
    val = data[getRelativeBitPos(i)];
    relPos = i-from;
    
    // take a correct value per bit
    if (relPos <= 3) {
      // from pos 0-3 calc with 2^x
      calc = (int)pow(2, relPos);
    } else {
      // from pos 4-x calc with 10*x
      calc = (relPos-3) * 10;
    }
    
    // printf("i: %d; relPos: %d; val: %d; calculated: %d\n", i, relPos, val, calc);
    
    result = result + val * calc;
  }
  return result;
}

char readDate() {
  char date[20] = "";
  
  int minute = readDCFNumber(21,27);
  int hour = readDCFNumber(29,34);
  int day = readDCFNumber(36,41);
  int month = readDCFNumber(45,49);
  int year = readDCFNumber(50,57);
  snprintf(date, sizeof date, "20%02d-%02d-%02dT%02d:%02d:00", year, month, day, hour, minute);
  
  printf("data: %s\n", date);
  
  return *date;
}


/**
 * try to read a valid date from the data
 * @param recursive should by try to call it self for error resolving
 * @return success decode (1 = succesfull) -  if it call it recursivle it will be always true
 */
int tryToReadData(int recursive) {
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
    printf("\nYEAH: found a valid date:\n");
    printData();
    
    // convert to date
    readDate();
    
    // reset pointer
    currentPointer = 0;
  }
  
  // if there a problem with the parity, try by the next value
  if (parityCheck == 0) {
    // printf("ou.. parity goes wrong - fix it (recursion: %d)\n", recursive);
    if (recursive == 1) {
      int tryAgain = 1;
      while (tryAgain == 1) {
        // remove first value
        
        // check abort condition
        if (currentPointer > 0) {
          // move first element out
          memmove(&data[0], &data[1], (BUFFER_ELEMENTS-1)*sizeof(*data));
          data[BUFFER_ELEMENTS-1] = 0;
          
          currentPointer--;
        
          // after re shift data, valid them again
          int tempCurrentPointer = currentPointer;
          // int valuesToCheck[] = { 20, 28, 35, 58 };
          int valuesToCheck[] = { 0, 8, 15, 38 };
          for (int i = 0; i <= 3; i++) {
            // only check value if there is anything in it
            if (valuesToCheck[i] <= tempCurrentPointer) {
              currentPointer = valuesToCheck[i];
              int tempSuccess = tryToReadData(0);
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
    success = 1;
  }
  
  // return tryAgain;
  return success;
}

void addReceivedByte(unsigned int received) {
  // printf("\n\ndebug_readPos: %d\n", debug_readPos);
  
  // debug_readPos++;
  int bit = 1;
  // magic mapping: http://wiki.gude.info/FAQ_mouseCLOCK_Programmierung
  if (received >= 192) {
    bit = 0;
  }
  
  // show something like a status bar
  if (currentPointer == 0) {
    printf("\nsearch a valid date:\n");
  }
  printf("*");
  
  data[currentPointer] = bit;
  currentPointer++;
  
  /*
  printf("\nread bit: %d\n", currentPointer);
  printData();
   */
  
  tryToReadData(1);
}


void dcf77_decode() {
  printf("start dcf77_decoder\n");
  
  /***** START - TEST DATE *****/
  
  // 120 predefined values (timestamp near: 23:10:50 DI 08.04.2014)) - by bit ≈43
  // full 2 mintues
  int temporaryData[] = { 252,252,252,0,252,252,252,0,252,252,252,128,252,252,252,252,128,252,128,252,252,252,0,252,254,252,128,0,128,128,128,252,128,252,252,252,254,128,252,254,0,254,254,128,252,252,252,252,128,252,252,128,0,128,252,252,252,128,128,252,254,254,0,252,252,254,0,254,252,252,128,252,252,252,252,128,252,0,254,254,254,128,254,0,252,0,128,128,252,252,252,252,252,254,0,254,128,254,252,128,252,252,128,128,252,254,252,128,252,252,252,0,128,252,254,252,128,128,252,252 };
  // full 2x2 times the signal
  // int temporaryData[] = { 252,252,252,0,252,252,252,0,252,252,252,128,252,252,252,252,128,252,128,252,252,252,0,252,254,252,128,0,128,128,128,252,128,252,252,252,254,128,252,254,0,254,254,128,252,252,252,252,128,252,252,128,0,128,252,252,252,128,128,252,254,254,0,252,252,254,0,254,252,252,128,252,252,252,252,128,252,0,254,254,254,128,254,0,252,0,128,128,252,252,252,252,252,254,0,254,128,254,252,128,252,252,128,128,252,254,252,128,252,252,252,0,128,252,254,252,128,128,252,252, 252,252,252,0,252,252,252,0,252,252,252,128,252,252,252,252,128,252,128,252,252,252,0,252,254,252,128,0,128,128,128,252,128,252,252,252,254,128,252,254,0,254,254,128,252,252,252,252,128,252,252,128,0,128,252,252,252,128,128,252,254,254,0,252,252,254,0,254,252,252,128,252,252,252,252,128,252,0,254,254,254,128,254,0,252,0,128,128,252,252,252,252,252,254,0,254,128,254,252,128,252,252,128,128,252,254,252,128,252,252,252,0,128,252,254,252,128,128,252,252 };
  // only valid data
  // int temporaryData[] = { 128,252,252,252,252,128,252,252,128,0,128,252,252,252,128,128,252,254,254,0,252,252,254,0,254,252,252,128,252,252,252,252,128,252,0,254,254,254,128,254 };
  // only the minutes
  // int temporaryData[] = { 128,252,252,252,252,128,252,252,128 };
  // only the minutes and hours
  // int temporaryData[] = { 128,252,252,252,252,128,252,252,128, 0,128,252,252,252,128,128 };
  /***** END - TEST DATE *****/
  
  int countValues = sizeof(temporaryData) / sizeof(*temporaryData);
  printf("sizeof(temporaryData): %u\n", countValues);
  for (int i = 0; i <= countValues; i++) {
    addReceivedByte(temporaryData[i]);
  }
  
  printf("end dcf77_decoder\n");
}