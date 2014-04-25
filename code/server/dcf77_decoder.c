/*
 * read the signal from the sensor
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
 *  2   every second an infos
 *  3   show invalid dates and error per each
 *  10  all (only for debugging)
 */
int debugLevel = 3;

/* defines the validation logic for a valid date
 *  0   no validation (only parity)
 *  1   check single date parts by householdknown (ex. minutes must be smaller than 60 etc.)
 *  2   check by current year
 *  3   check by current year, month
 *  4   check by current year, month, day
 */
int dateValidatorLevel = 4;

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

/**
 * read a data from the stored bits
 * @return pointer to the date
 */
char readDate() {
  char date[20] = "";
  
  int minute = readDCFNumber(21,27);
  int hour = readDCFNumber(29,34);
  int day = readDCFNumber(36,41);
  int month = readDCFNumber(45,49);
  int year = readDCFNumber(50,57);
  
  // check all date parts by household known
  int dateIsValid = 1;
  char errorMessage[200] = "";
  if (dateValidatorLevel >= 1) {
    if (minute > 59) {
      dateIsValid = 0;
      strcat(errorMessage, "invalid minute; ");
    }
    if (hour > 23) {
      dateIsValid = 0;
      strcat(errorMessage, "invalid hour; ");
    }
    if (day > 31) {
      dateIsValid = 0;
      strcat(errorMessage, "invalid day; ");
    }
    if (month > 12) {
      dateIsValid = 0;
      strcat(errorMessage, "invalid month; ");
    }
    if (year > 99) {
      dateIsValid = 0;
      strcat(errorMessage, "invalid year; ");
    }
  }
  
  // get time information
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  
  // check year to current year
  if (dateValidatorLevel >= 2) {
    // year since 1900
    if (timeinfo->tm_year != year + 100) {
      dateIsValid = 0;
      strcat(errorMessage, "year doesn't match; ");
    }
  }
  
  // check month to current month
  if (dateValidatorLevel >= 3) {
    // month (0 – 11, 0 = January)
    if (timeinfo->tm_mon != month-1) {
      dateIsValid = 0;
      strcat(errorMessage, "month doesn't match; ");
    }
  }
  
  // check month to current day
  if (dateValidatorLevel >= 4) {
    if (timeinfo->tm_mday != day) {
      dateIsValid = 0;
      strcat(errorMessage, "day doesn't match; ");
    }
  }
  
  // write date in iso
  snprintf(date, sizeof date, "20%02d-%02d-%02dT%02d:%02d:00", year, month, day, hour, minute);
  
  if (dateIsValid == 1) {
    if (debugLevel >= 1) {
      printf("\nYEAH: found a valid date by parity:\n");
      printData();
      
      printf("date\t%s\n", date);
      printf("\tdecoded at %s\treadPos last bit\tabs: %d\trel: %d\n", asctime(timeinfo), debug_readPos, debug_readPos%59);
    }
  } else {
    if (debugLevel >= 3) {
      printf("\tDATE IS CRAP:\t%s\n", date);
      printf("\error reason:\t%s\n", errorMessage);
    }
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
  int foundValidDate_parity = 0;
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
          foundValidDate_parity = 1;
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
  
  if (foundValidDate_parity == 1) {
    // found a valid data
    
    // convert to date
    readDate();
    
    // force new search
    parityCheck = 0;
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
  if (received > 192) {
    bit = 0;
  }
  
  if (debugLevel >= 10) {
    printf("\n\n===\ndebug_readPos: %d\tcurrentPointer: %d\tvalue: %d\n", debug_readPos, currentPointer, bit);
  }
  debug_readPos++;
  
  if (debugLevel >= 2) {
    printf("Total: %04d\tRel: %02d\t\tSignal: %02d/39\tvalue: %d\t%d\n", debug_readPos, debug_readPos%59, currentPointer+1, bit, received);
  }
  
  // fflush(stdout); // print every process
  
  data[currentPointer] = bit;
  
  tryToReadData(1);
  currentPointer++;
}


void dcf77_decode() {
  printf("start dcf77_decoder\n");
  
  /***** START - TEST DATE *****/
  
  // xx
  int temporaryData[] = {  };
  
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