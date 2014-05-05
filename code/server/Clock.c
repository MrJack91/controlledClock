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
#include <time.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>

/*---------------------------- Includes: User-Libs ---------------------------*/
#include "clock.h"

/*---------------------------- Declarations ----------------------------------*/

#define TIMER_RESOLUTION 1 

TimeStruct currentTime;
TimeStruct lastSyncTime;

int ticsPerSecond;
int currentTics;

int synched;

sem_t* clockSem;
sem_t* timerSem;
sem_t* syncSem;
/*---------------------------- Internal functions ----------------------------*/

void loadFromSystem();

void tic();

void timerHandler(int signum);

/*---------------------------- Implementations -------------------------------*/
void clock_start(){
    // if (sem_init(&clockSem, 0, 1) != 0) {
    clockSem = sem_open("/clockSemaphore", O_CREAT, 0, 1);
    if (clockSem == SEM_FAILED) {
        perror("Clock Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }
    
    // if (sem_init(&timerSem, 0, 1) != 0) {
    timerSem = sem_open("/timerSemaphore", O_CREAT, 0, 1);
    if (timerSem == SEM_FAILED) {
        perror("Timer Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }
    
    // if (sem_init(&syncSem, 0, 1) != 0) {
    syncSem =  sem_open("/syncSemaphore", O_CREAT, 0, 1);
    if (syncSem == SEM_FAILED) {
        perror("Sync Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }
    
    loadFromSystem();
    
    ticsPerSecond = 1;
    currentTics = 0;
    synched = 0;
    
    siginterrupt(SIGALRM, TIMER_RESOLUTION);
    
    signal (SIGALRM, timerHandler);
    alarm (TIMER_RESOLUTION);
    
}

void clock_shutdown(){
  /*
    sem_destroy(&clockSem);
    sem_destroy(&timerSem);
    sem_destroy(&syncSem);
    */
  
  if (sem_close(clockSem) == -1) {
    perror("sem_close: clockSemaphore");
    exit(EXIT_FAILURE);
  }
  if (sem_unlink("/clockSemaphore") == -1) {
    perror("sem_unlink: clockSemaphore");
    exit(EXIT_FAILURE);
  }
  
  if (sem_close(timerSem) == -1) {
    perror("sem_close: timerSemaphore");
    exit(EXIT_FAILURE);
  }
  if (sem_unlink("/timerSemaphore") == -1) {
    perror("sem_unlink: timerSemaphore");
    exit(EXIT_FAILURE);
  }
  
  if (sem_close(syncSem) == -1) {
    perror("sem_close: syncSemaphore");
    exit(EXIT_FAILURE);
  }
  if (sem_unlink("/syncSemaphore") == -1) {
    perror("sem_unlink: syncSemaphore");
    exit(EXIT_FAILURE);
  }
}

TimeStruct clock_getCurrentTime(){
    return currentTime;
}

TimeStruct clock_getLastSyncTime(){
    return lastSyncTime;
}

void clock_syncTime(TimeStruct aTime){
    sem_wait(syncSem);
    sem_wait(timerSem);
    sem_wait(clockSem);
   
    //cancel alarm
    siginterrupt(SIGALRM, 0);
    // alarm(0);
    
    //setup next alarm
    alarm (TIMER_RESOLUTION);
     
    synched = 1;
    
    //Sync time
    currentTics = 0;
    currentTime = aTime;
    
    lastSyncTime.year = currentTime.year;
    lastSyncTime.month = currentTime.month;
    lastSyncTime.day = currentTime.day;
    lastSyncTime.hour = currentTime.hour;
    lastSyncTime.minute = currentTime.minute;
    lastSyncTime.second = currentTime.second;
    
    sem_post(clockSem);
    sem_post(timerSem);
    sem_post(syncSem);
}
    
void clock_ticSecond(){
     //Abort if sync is in process
    int syncSemVal;
    sem_getvalue(syncSem,&syncSemVal);
    
    if(syncSemVal == 0){
        return;
    }
    
    sem_wait(timerSem);
    sem_wait(clockSem);
    
    //cancel alarm
    alarm(0);
    
    //setup next alarm
    alarm (TIMER_RESOLUTION);

    currentTics = ticsPerSecond - 1;
    sem_post(clockSem);
    sem_post(timerSem);
    
    tic();
}

void timerHandler(int signum){
    //Abort if sync is in process
    int syncSemVal;
    sem_getvalue(syncSem,&syncSemVal);
    
    if(syncSemVal == 0){
        return;
    }
    
    sem_wait(timerSem);
       
    alarm (TIMER_RESOLUTION);

    tic();
    sem_post(timerSem);
}

void tic(){
    sem_wait(clockSem);

    currentTics++;

    //Second passed
    if(currentTics >= ticsPerSecond){
        currentTime.second++;

        if(currentTime.second == 60){
            currentTime.second = 0;
            currentTime.minute++;

            if(currentTime.minute == 60){
                currentTime.minute = 0;
                currentTime.hour++;

                if(currentTime.hour == 24){
                    currentTime.hour = 0;
                    currentTime.day++;

                    int maxDay = 31;
                    int daysInYear = 365;

                    switch(currentTime.month){
                        case 4:
                        case 6:
                        case 9:
                        case 11:
                            maxDay = 30;
                            break;
                        case 2:
                            if((((currentTime.year % 4) == 0) && !((currentTime.year % 100) == 0)) || (((currentTime.year % 4) == 0) && ((currentTime.year % 400) == 0))){
                                maxDay = 29;
                                daysInYear = 366;
                            }else{
                                maxDay = 28;
                            }
                            break;
                    }

                    if(currentTime.day > maxDay){
                        currentTime.day = 1;
                        currentTime.month++;

                        if(currentTime.month > 12){
                            currentTime.month = 1;
                            currentTime.year++;
                        }
                    }
                }
            }
        }
    }
    
    sem_post(clockSem);
}

void loadFromSystem(){
    time_t t = time(NULL);
    struct tm lt = *localtime(&t);
    
    //TODO: Use Sempaphores
    lastSyncTime.year = (lt.tm_year + 1900);
    lastSyncTime.month = (lt.tm_mon + 1);
    lastSyncTime.day = lt.tm_mday;
    lastSyncTime.hour = lt.tm_hour;
    lastSyncTime.minute = lt.tm_min;
    lastSyncTime.second = lt.tm_sec;
    
    if(lt.tm_isdst == 0){
         lastSyncTime.zoneOffset = 1;
    }else{
         lastSyncTime.zoneOffset = 2;
    }
    
    //TODO: Use Sempaphores
    currentTime.year = (lt.tm_year + 1900);
    currentTime.month = (lt.tm_mon + 1);
    currentTime.day = lt.tm_mday;
    currentTime.hour = lt.tm_hour;
    currentTime.minute = lt.tm_min;
    currentTime.second = lt.tm_sec;
    
    if(lt.tm_isdst == 0){
         currentTime.zoneOffset = 1;
    }else{
         currentTime.zoneOffset = 2;
    }
}