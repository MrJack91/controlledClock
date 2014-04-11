/* 
 * File:   clock.c
 * Author: Daniel Brun
 * Created: 10.04.2014
 * 
 * Description: 
 * Provides a clock function which relays on the system tic.
 *  
 */

/*---------------------------- Includes: System ------------------------------*/
#include <stdio.h>
#include <stdlib.h>

/*---------------------------- Includes: User-Libs ---------------------------*/
#include "clock.h"

/*---------------------------- Declarations ----------------------------------*/
TimeStruct currentTime;
TimeStruct lastSyncTime;

/*---------------------------- Internal functions ----------------------------*/

unsigned long long rdtsc();


/*---------------------------- Implementations -------------------------------*/
void clock_start(){
    currentTime.year = 2014;
    currentTime.month = 4;
    currentTime.day = 10;
    currentTime.hour = 18;
    currentTime.minute = 10;
    currentTime.second = 5;
    currentTime.zoneOffset = 1;
    
    lastSyncTime.year = 2014;
    lastSyncTime.month = 4;
    lastSyncTime.day = 10;
    lastSyncTime.hour = 18;
    lastSyncTime.minute = 10;
    lastSyncTime.second = 5;
    lastSyncTime.zoneOffset = 1; 
    
    printf("%lld\n",rdtsc());
    printf("%lld\n",rdtsc());
    printf("%lld\n",rdtsc());
    
    //High res claendar: http://www.chemie.fu-berlin.de/chemnet/use/info/libc/libc_17.html
}

unsigned long long rdtsc()
{
    //http://stackoverflow.com/questions/275004/c-timer-function-to-provide-time-in-nano-seconds
  #define rdtsc(low, high) \
         __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

  unsigned int low, high;
  rdtsc(low, high);
  return ((long long)high << 32) | low;
}

void clock_shutdown(){
    
}

TimeStruct clock_getCurrentTime(){
    return currentTime;
}

TimeStruct clock_getLastSyncTime(){
    return lastSyncTime;
}