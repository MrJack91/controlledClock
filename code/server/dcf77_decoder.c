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
#include <time.h>
#include <math.h> // to use the pow command

// max count of element in buffer
#define BUFFER_ELEMENTS 100

// current received data (relative number bit-20) - buffer/cache for 100 numbers
unsigned int data[BUFFER_ELEMENTS];

// calc buffersize for 100 ints
int BUFFER_SIZE = BUFFER_ELEMENTS* sizeof(unsigned int);

int currentPointer = 0;

int debug_readPos = 0;


/* defines the output for debugging
 *  0   nothing
 *  1   only found valid dates
 *  2   every second infos
 *  10  all (only for debugging)
 */
int debugLevel = 2;

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
  int i = 0;
  for (i = 0; i <= currentPointer; i++) {
    printf("%d ", data[i]);
  }

  printf(" - (length: %d)\n", currentPointer+1);
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
  int i = 0;
  for (i = from; i <= to; i++) {
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
  int i = 0;
  for (i = from; i <= to; i++) {
    val = data[getRelativeBitPos(i)];
    relPos = i-from;
    
    // take a correct value per bit
    if (relPos <= 3) {
      // from pos 0-3 calc with 2^x
      calc = (int)pow(2, relPos);
    } else {
      // from pos 4-x calc with 10*x
      calc = (int)pow(2, relPos-4) * 10;
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
  
  if (debugLevel >= 1) {
    snprintf(date, sizeof date, "20%02d-%02d-%02dT%02d:%02d:00", year, month, day, hour, minute);
    printf("date\t%s\n", date);

    // get time information
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf("\tdecoded at %s\treadPos last bit: %d\n", asctime (timeinfo), debug_readPos);
  }
  
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
    switch (currentPointer) {
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
  
  if (debugLevel >= 10) {
    printf("parity check: %d\n", parityCheck);
  }
  
  if (foundValidDate == 1) {
    // found a valid data
    if (debugLevel >= 1) {
      printf("\nYEAH: found a valid date:\n");
      printData();
    }
    
    // convert to date
    readDate();
    
    // reset pointer
    // currentPointer = 0;
    
    // NOT SURE! After all i think it's correct, but have to check parity again!!
    // try to search more results in one minute
    // remove first bit and set pointer --
    // printf("search all")
    // printData();
    /*
    memmove(&data[0], &data[1], (BUFFER_ELEMENTS-1)*sizeof(*data));
    data[BUFFER_ELEMENTS-1] = 0;
    currentPointer = currentPointer - 1;
    */
    // force new search
    parityCheck = 0;
    // printData();
  }
  
  // if there a problem with the parity, try by the next value
  if (parityCheck == 0) {
    if (debugLevel >= 10) {
      printf("ou.. parity goes wrong - fix it (recursion: %d)\n", recursive);
    }
    
    if (recursive == 1) {
      int tryAgain = 1;
      while (tryAgain == 1) {
        
        if (debugLevel >= 10) {
          printf("\nin while with counter: %d\n", currentPointer);
          printData();
        }
          
        // move first element out
        memmove(&data[0], &data[1], (BUFFER_ELEMENTS-1)*sizeof(*data));
        data[BUFFER_ELEMENTS-1] = 0;
        currentPointer--;
        
        if (debugLevel >= 10) {
          printData();
        }
        
        // check abort condition
        if (currentPointer > 0) {
          // after re shift data, valid them again
          int tempCurrentPointer = currentPointer;
          int valuesToCheck[] = { 0, 8, 15, 38 };
          int i = 0;
          for (i = 0; i <= 3; i++) {
            if (debugLevel >= 10) {
              printf("i was %d\n", i);
            }
            
            // only check value if there is anything in it
            if (tempCurrentPointer >= valuesToCheck[i]) {
              currentPointer = valuesToCheck[i];
              int tempSuccess = tryToReadData(0);
              if (debugLevel >= 10) {
                printf("i %d was checked - result: %d\n", i, tempSuccess);
              }
              if (tempSuccess == 0) {
                // there was a parity error -> break and try again with next bit
                tryAgain = 1;
                break; // leave check parity for this sequence
              }
            } else {
              if (debugLevel >= 10) {
                printf("skip check for %d\n", i);
              }
              
              // this test was skipped, because not enough data
              tryAgain = 0;
              success = 1; // so all it's fine
              break; // leave check parity for this sequence
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
  
  if (debugLevel >= 10) {
    printf("success %d\n", success);
  }
  
  // return tryAgain;
  return success;
}

void addReceivedByte(unsigned int received) {
  int bit = 1;
  // magic mapping: http://wiki.gude.info/FAQ_mouseCLOCK_Programmierung
  if (received >= 192) {
    bit = 0;
  }
  
  if (debugLevel >= 10) {
    printf("\n\n===\ndebug_readPos: %d\tcurrentPointer: %d\tvalue: %d\n", debug_readPos, currentPointer, bit);
  }
  debug_readPos++;
  
  // show something like a status bar (not correct because recursive modification)
  if (currentPointer == 0) {
    // printf("\nsearch a valid date: (length 39)   to |\n");
  }
  // printf("*");
  
  if (debugLevel >= 2) {
    printf("Total: %04d\tRel: %02d\t\tSignal: %02d/39\n", debug_readPos, debug_readPos%60, currentPointer+1);
  }
  
  fflush(stdout); // print every process
  // printf("abs: %d; rel: %d\n", debug_readPos, currentPointer);
  
  data[currentPointer] = bit;
  // currentPointer++;
  
  // printf("\nread bit: %d\n", currentPointer);
  // printData();
  
  tryToReadData(1);
  currentPointer++;
}


void dcf77_decode() {
  printf("start dcf77_decoder\n");
  
  /***** START - TEST DATE *****/
  
  // 900 values (10 minutes) - try 1
  int temporaryData[] = { 128,252,252,252,128,252,252,252,0,252,128,252,128,252,252,128,252,252,252,252,128,254,128,252,252,252,128,252,252,252,254,128,128,128,252,254,254,128,128,252,252,128,252,252,128,254,252,128,252,252,128,252,252,254,252,128,128,252,252,0,252,254,254,128,254,252,252,128,252,128,252,128,252,252,0,252,252,252,254,128,252,128,252,252,252,0,252,128,128,254,252,252,252,252,128,128,128,0,252,252,254,252,254,128,252,252,128,128,254,128,252,252,254,252,254,128,252,254,128,252,252,252,128,252,254,252,128,252,128,252,128,252,252,128,254,254,252,252,128,254,128,252,252,252,0,252,128,0,128,128,254,252,128,252,252,128,252,128,252,128,252,252,128,252,254,128,254,128,128,252,252,254,252,254,128,252,254,128,252,254,252,128,252,252,252,128,252,128,252,0,252,252,0,252,252,252,254,128,252,128,252,252,252,0,252,252,128,128,252,252,252,252,252,252,252,128,128,252,128,254,252,0,252,252,128,128,128,128,252,252,252,252,128,128,252,252,128,254,252,252,128,254,254,252,128,254,128,252,128,252,252,128,252,252,252,252,128,252,128,252,252,254,128,252,252,252,252,254,128,252,252,252,128,252,252,128,128,252,252,252,128,252,254,128,252,252,252,128,252,252,254,128,128,252,240,0,252,252,254,128,252,252,252,128,252,128,252,128,252,252,128,254,254,252,254,128,254,128,252,252,252,128,252,252,128,128,254,128,0,128,252,252,128,128,254,252,254,254,252,128,252,254,128,128,254,252,128,252,252,252,252,128,252,252,128,254,254,254,128,252,252,252,128,252,128,252,128,252,252,128,252,252,252,254,0,252,128,252,252,252,128,252,252,252,252,252,252,0,128,252,252,128,128,252,254,128,254,254,128,254,252,128,252,252,254,252,0,252,252,0,0,252,252,0,254,252,252,128,252,252,252,0,252,128,252,128,252,254,128,252,252,252,252,128,252,128,252,252,252,128,252,252,252,128,128,128,252,128,128,252,252,128,128,252,128,252,252,128,252,252,128,128,252,252,252,128,252,252,252,0,254,252,0,252,252,252,128,252,252,252,128,254,128,252,128,252,252,128,252,252,252,252,128,252,128,252,252,252,128,252,252,128,252,254,128,128,128,128,128,128,252,128,252,252,254,254,128,252,252,128,252,128,252,254,0,252,252,252,128,254,252,128,252,252,252,128,252,252,254,128,252,128,252,128,252,252,128,252,252,252,252,128,252,128,252,252,252,128,252,252,128,252,252,128,128,128,254,252,254,128,128,128,128,252,254,128,254,252,128,128,128,254,252,128,254,252,128,128,252,254,128,252,252,252,128,252,254,252,128,252,128,254,128,252,252,128,252,252,252,254,128,252,128,252,252,252,128,252,128,128,128,254,254,252,252,128,128,128,128,128,252,0,252,252,128,252,252,128,252,252,128,252,128,254,252,252,128,252,252,128,252,252,254,128,252,252,252,128,252,128,252,128,252,252,128,252,252,254,252,128,252,128,252,252,252,128,252,128,252,252,252,252,128,254,128,128,128,128,0,252,128,254,252,128,252,252,128,128,252,128,252,128,252,252,128,128,252,252,0,252,252,252,0,252,252,252,128,252,128,252,128,252,252,128,252,252,252,252,128,252,128,254,252,254,128,252,252,252,0,0,128,128,128,252,252,252,254,252,128,252,252,252,128,252,252,128,252,128,128,252,128,252,252,128,128,254,252,128,252,252,252,128,252,252,252,128,252,128,252,128,252,252,128,254,252,252,252,128,252,0,252,252,254,128,252,254,252,128,254,128,252,128,128,252,128,128,252,252,254,252,252,128,252,252,128,128,128,128,252,128,254,252,252,0,252,252,128,252,252,252,128,252,254,252,128,252,128,252,128,252,252,128,252,252,252,252,128,252,128,252,252,252,128,252,128,128,128,0,128,128,0,252,128,252,128,128,128,252,252,252,128,252,254,128,252,252,252,128,128,252,254,252,128,252,252,128,252,252,252,128,252,252,252,128,252,128,254,128,252,252 };
  
  // try 2 - 18:56 11.04 2014
  // int temporaryData[] = { 254,254,128,0,254,252,128,252,252,252,128,252,0,252,128,252,254,0,252,252,240,254,128,248,128,252,254,252,192,252,252,192,128,252,0,128,128,128,128,128,128,252,252,128,254,252,128,252,252,128,128,252,192,252,252,254,128,0,252,254,254,128,128,254,252,128,252,254,254,0,252,128,252,128,252,252,128,252,252,252,254,128,252,128,254,254,252,128,254,252,0,128,128,252,252,128,252,128,128,254,128,0,252,252,252,128,252,252,128,252,0,128,254,254,252,128,128,252,252,252,0,128,254,252,128,252,252,254,128,252,192,252,128,254,252,128,252,254,254,252,128,252,192,254,254,254,0,254,0,128,0,254,128,128,254,0,128,0,252,254,128,128,252,252,0,254,254,128,0,0,128,255,254,252,128,254,252,252,252,128,128,254,252,128,252,255,252,128,254,128,254,0,254,254,0,254,240,252,252,128,252,128,252,254,252,128,254,252,254,252,252,128,252,128,254,252,252,128,128,128,255,254,252,128,254,252,128,252,255,252,128,252,252,128,252,254,254,254,128,128,252,254,192,254,252,252,0,254,0,254,192,252,128,255,252,252,254,128,254,128,252,252,252,0,254,252,128,0,252,254,252,128,252,254,252,192,128,128,128,252,254,0,252,252,128,0,252,252,0,252,254,128,0,254,252,254,128,128,254,254,128,252,248,254,0,252,0,252,0,254,254,192,252,252,254,254,128,255,128,252,252,252,128,252,252,128,252,252,252,128,128,128,128,252,248,0,128,128,252,254,128,254,254,128,252,252,252,252,128,254,128,252,254,252,254,128,0,252,252,128,254,254,252,128,254,128,254,128,252,252,0,252,252,255,252,0,254,128,254,248,252,0,252,128,252,128,255,252,128,128,128,128,128,128,252,254,0,252,252,128,252,254,128,128,252,252,255,0,252,128,128,252,252,254,128,128,252,252,128,254,252,252,0,254,128,254,128,252,254,128,252,254,252,252,128,240,0,254,254,254,128,252,252,252,254,252,252,252,252,252,0,128,128,0,192,128,254,254,128,252,252,128,252,128,252,252,192,254,128,128,254,254,252,128,128,254,252,128,252,252,252,128,252,128,252,128,252,252,0,252,254,254,252,128,252,0,252,252,252,128,252,128,128,128,192,252,252,128,255,252,128,252,128,255,128,252,254,128,252,255,128,128,128,240,254,0,252,128,252,252,252,254,192,128,252,254,0,252,252,254,128,252,0,252,128,252,252,128,252,252,254,254,192,254,128,254,252,254,128,252,254,254,252,252,128,254,0,192,252,252,254,254,128,0,252,252,128,252,254,0,252,0,252,128,254,0,128,252,252,252,128,192,252,252,128,252,252,254,0,252,128,254,0,254,252,128,252,252,254,252,128,254,128,240,252,252,0,252,252,128,0,252,254,0,254,252,0,252,128,128,252,0,254,252,128,252,254,128,192,252,0,254,128,252,128,252,254,254,254,128,128,254,254,128,254,252,254,0,254,128,252,128,252,254,0,254,254,252,252,128,252,128,254,252,252,128,252,254,252,128,254,0,128,254,254,128,128,254,252,254,252,252,252,128,252,252,128,252,0,128,254,0,252,128,255,254,252,252,128,128,252,252,128,252,254,252,128,252,0,252,128,252,252,128,252,252,252,252,128,252,0,252,252,252,128,240,128,252,0,252,252,128,128,0,128,128,0,128,254,252,252,252,128,252,252,128,192,192,0,254,224,254,128,128,254,252,252,128,128,252,254,128,252,252,252,128,252,192,254,128,254,254,128,252,252,252,252,128,252,0,254,252,254,0,252,252,252,252,128,252,252,128,254,128,128,254,254,254,254,254,254,128,252,240,128,252,252,252,128,128,252,128,128,252,252,252,128,128,252,252,192,254,252,254,0,252,128,254,128,252,252,0,254,252,252,252,192,252,128,252,252,252,192,252,254,252,252,252,252,192,128,254,252,192,128,128,254,254,252,252,128,252,254,128,128,252,252,0,128,252,0,254,254,255,252,128,128,254,254,0,254,255,255,128,240,128,254,0,254,255 };
  
  // int temporaryData[] = { 128,252,252,252,128,252,252,252,0,252,128,252,128,252,252,128,252,252,252,252,128,254,128,252,252,252,128,252,252,252,254,128,128,128,252,254,254,128,128,252,252,128,252,252,128,254,252,128,252,252,128,252,252,254,252,128,128,252,252,0,252,254,254,128,254,252,252,128,252,128,252,128,252,252,0,252,252,252,254,128,252,128,252,252,252,0,252,128,128,254,252,252,252,252,128,128,128,0,252,252,254,252,254,128,252,252,128,128,254,128,252,252,254,252,254,128,252,254,128,252,252,252,128,252,254,252,128,252,128,252,128,252,252,128,254,254,252,252,128,254,128,252,252,252,0,252,128,0,128,128,254,252,128,252,252,128,252,128,252,128,252,252,128,252,254,128,254,128,128,252,252,254,252,254,128,252,254,128,252,254,252,128,252,252,252,128,252,128,252,0,252,252,0,252,252,252,254,128,252,128,252,252,252,0,252,252,128,128,252,252,252,252,252,252,252,128,128,252,128,254,252,0,252,252,128,128,128,128,252,252,252,252,128,128,252,252,128,254,252,252,128,254,254,252,128,254,128,252,128,252,252,128,252,252,252,252,128,252,128,252,252,254,128,252,252,252,252,254,128,252,252,252,128,252,252,128,128,252,252,252,128,252,254,128,252,252,252,128,252,252, };
  
  // the magical input (error in last bit, one move then it's correct)
  // int temporaryData[] = { 0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,255,0,0,255,0,0,0,255,255,0,0,0,0,255,0,255,255,255,255, 0 };

  
  // 120 predefined values (timestamp near: 23:10:50 DI 08.04.2014)) - by bit ≈43
  // full 2 mintues
  // int temporaryData[] = { 252,252,252,0,252,252,252,0,252,252,252,128,252,252,252,252,128,252,128,252,252,252,0,252,254,252,128,0,128,128,128,252,128,252,252,252,254,128,252,254,0,254,254,128,252,252,252,252,128,252,252,128,0,128,252,252,252,128,128,252,254,254,0,252,252,254,0,254,252,252,128,252,252,252,252,128,252,0,254,254,254,128,254,0,252,0,128,128,252,252,252,252,252,254,0,254,128,254,252,128,252,252,128,128,252,254,252,128,252,252,252,0,128,252,254,252,128,128,252,252 };
  
  // temporary: 
  // int temporaryData[] = { 252,252,252,0,252,252,252,0,252,252,252,128,252,252,252,252,128,252,128,252,252,252,0,252,254,252,128,0,128,128,128,252,128,252,252,252,254,128,252,254,0,254,254,128,252,252,252,252,128,252,252,128,0,128,252,252,252,128,128,252,254,254,0,252,252,254,0,254,252,252,128,252,252,252,252,128,252,0,254,254,254,128,254,0,252,0,128,128,252,252,252,252,252,254,0,254,128,254,252,128,252,252,128,128,252,254,252,128,252,252,252,0,128,252,254,252,128,128,252,252 };
  
  
  // temporary: only 60 values -> start of signal by 3.
  // int temporaryData[] = { 252,254,254,0,252,252,254,0,254,252,252,128,252,252,252,252,128,252,0,254,254,254,128,254,0,252,0,128,128,252,252,252,252,252,254,0,254,128,254,252,128,252,252,128,128,252,254,252,128,252,252,252,0,128,252,254,252,128,128,252 };
  
  
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
  
  int i = 0;
  for (i = 0; i <= countValues; i++) {
    addReceivedByte(temporaryData[i]);
  }
  printf("\n");
  
  printf("end dcf77_decoder\n");
}