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
#include <unistd.h>
#include <pthread.h>

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

int verifyTime(TimeStruct aTime);

int getLastDayOfMonth(int year, int aMonth);

/*---------------------------- Implementations -------------------------------*/
TimeStruct clock_getCurrentTime() {
  return currentTime;
}

TimeStruct clock_getLastSyncTime() {
  return lastSyncTime;
}

void clock_startSigalrm() {
  struct sigaction sa;
  sigset_t mask;

  sa.sa_handler = &timerHandler; // Intercept and ignore SIGALRM
  sa.sa_flags = SA_RESTART;
  sigfillset(&sa.sa_mask);
  sigaction(SIGALRM, &sa, NULL);

  // Get the current signal mask
  sigprocmask(0, NULL, &mask);

  // Unblock SIGALRM
  sigdelset(&mask, SIGALRM);

  // Wait with this mask
  alarm(TIMER_RESOLUTION);
  sigsuspend(&mask);

  printf("sigsuspend() returned\n");
}

void timerHandler(int signum) {
  //Abort if sync is in process
  int syncSemVal;
  sem_getvalue(syncSem, &syncSemVal);
  
  // printf("timer handler is called (with syncSem: %d)\n", syncSemVal);
  
  // avoid to disturb a sync process
  if (syncSemVal < 0) {
    return;
  }

  sem_wait(timerSem);

  alarm(TIMER_RESOLUTION);

  // printf("%02d:%02d:%02d", currentTime.hour, currentTime.minute, currentTime.second);

  tic();

  // printf("%02d:%02d:%02d", currentTime.hour, currentTime.minute, currentTime.second);


  sem_post(timerSem);
}

void tic() {
  sem_wait(clockSem);

  currentTics++;

  //Second passed
  if (currentTics >= ticsPerSecond) {
    currentTime.second++;

    if (currentTime.second == 60) {
      currentTime.second = 0;
      currentTime.minute++;

      if (currentTime.minute == 60) {
        currentTime.minute = 0;
        currentTime.hour++;

        if (currentTime.hour == 24) {
          currentTime.hour = 0;
          currentTime.day++;

          int maxDay = getLastDayOfMonth(currentTime.year, currentTime.month);

          if (currentTime.day > maxDay) {
            currentTime.day = 1;
            currentTime.month++;

            if (currentTime.month > 12) {
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

int getLastDayOfMonth(int year, int aMonth) {
    int maxDay = 31;

    switch (aMonth) {
        case 4:
        case 6:
        case 9:
        case 11:
            maxDay = 30;
            break;
        case 2:
            if ((((year % 4) == 0) && !((year % 100) == 0)) || (((year % 4) == 0) && ((year % 400) == 0))) {
                maxDay = 29;
            } else {
                maxDay = 28;
            }
            break;
    }
    return maxDay;
}

int verifyTime(TimeStruct aTime){
    int result = -1;
    
    if  ((aTime.year == currentTime.year) || 
        (aTime.year == currentTime.year - 1 && aTime.month == 12 && aTime.day == 31) || 
        (aTime.year == currentTime.year + 1 && aTime.month == 1 && aTime.day == 1)) {

        int lastDayOfMonth = getLastDayOfMonth(aTime.year, aTime.month);
        if  ((aTime.month == currentTime.month) || 
            (aTime.month == currentTime.month - 1 && aTime.day == lastDayOfMonth) ||
            (aTime.month == currentTime.month + 1 && aTime.day == 1)){
            
            if  ((aTime.day == currentTime.day) ||
                (aTime.day == currentTime.day - 1 && aTime.hour == 23) ||
                (aTime.day == currentTime.day + 1 && aTime.hour == 0)){
                
                if  ((aTime.hour == currentTime.hour) ||
                    (aTime.hour >= currentTime.hour-2) ||
                    (aTime.hour <= currentTime.hour+2)){
                    result = 0;
                }
            }
        }
    }
   
    return result;
}

void clock_syncTime(TimeStruct aTime) {
  sem_wait(syncSem);
  sem_wait(timerSem);
  sem_wait(clockSem);

  if ( verifyTime(aTime) == 0){
    //cancel alarm
    // siginterrupt(SIGALRM, 0);
    alarm(0);

    //setup next alarm
    alarm(TIMER_RESOLUTION);
  
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
  }
  
  sem_post(clockSem);
  sem_post(timerSem);
  sem_post(syncSem);
}

void loadFromSystem() {
  time_t t = time(NULL);
  struct tm lt = *localtime(&t);

  // set timezone of last sync
  if (lt.tm_isdst == 0) {
    lastSyncTime.zoneOffset = 1;
  } else {
    lastSyncTime.zoneOffset = 2;
  }

  currentTime.year = (lt.tm_year + 1900);
  currentTime.month = (lt.tm_mon + 1);
  currentTime.day = lt.tm_mday;
  currentTime.hour = lt.tm_hour;
  currentTime.minute = lt.tm_min;
  currentTime.second = lt.tm_sec;

  if (lt.tm_isdst == 0) {
    currentTime.zoneOffset = 1;
  } else {
    currentTime.zoneOffset = 2;
  }

  printf("Time on server is initialised: %02d:%02d:%02d\n", currentTime.hour, currentTime.minute, currentTime.second);
}

void clock_start() {
  atexit(clock_shutdown);

    //unlink existing semaphores
    if (sem_unlink("/clockSemaphore") == -1) {
        perror("sem_unlink: clockSemaphore\n");
    }

    if (sem_unlink("/timerSemaphore") == -1) {
        perror("sem_unlink: timerSemaphore\n");
    }

    if (sem_unlink("/syncSemaphore") == -1) {
        perror("sem_unlink: syncSemaphore\n");
    }
  
  // if (sem_init(&clockSem, 0, 1) != 0) {
  clockSem = sem_open("/clockSemaphore", O_CREAT, 0, 1);
  if (clockSem == SEM_FAILED) {
    perror("Clock Semaphore initialization failed\n");
    pthread_exit((void*)EXIT_FAILURE);
  }

  // if (sem_init(&timerSem, 0, 1) != 0) {
  timerSem = sem_open("/timerSemaphore", O_CREAT, 0, 1);
  if (timerSem == SEM_FAILED) {
    perror("Timer Semaphore initialization failed\n");
    pthread_exit((void*)EXIT_FAILURE);
  }

  // if (sem_init(&syncSem, 0, 1) != 0) {
  syncSem = sem_open("/syncSemaphore", O_CREAT, 0, 1);
  if (syncSem == SEM_FAILED) {
    perror("Sync Semaphore initialization failed\n");
    pthread_exit((void*)EXIT_FAILURE);
  }

  loadFromSystem();

  ticsPerSecond = 1;
  currentTics = 0;
  synched = 0;

  // siginterrupt(SIGALRM, 1);

  signal(SIGALRM, timerHandler);
  alarm(TIMER_RESOLUTION);
  
  // clock_startSigalrm();
}

void clock_shutdown() {
  printf("Shutdown Clock... \n");
  
  sem_wait(syncSem);
  sem_wait(timerSem);
  sem_wait(clockSem);
  
  alarm(0);
  
  if(clockSem > 0){
    if (sem_close(clockSem) == -1) {
      perror("sem_close: clockSemaphore\n");
    }
    if (sem_unlink("/clockSemaphore") == -1) {
      perror("sem_unlink: clockSemaphore\n");
    }
  }
  sem_post(syncSem);
  
  if(timerSem > 0){
    if (sem_close(timerSem) == -1) {
      perror("sem_close: timerSemaphore\n");
    }
    if (sem_unlink("/timerSemaphore") == -1) {
      perror("sem_unlink: timerSemaphore\n");
    }
  }

  if(syncSem > 0){
    if (sem_close(syncSem) == -1) {
      perror("sem_close: syncSemaphore\n");
    }
    if (sem_unlink("/syncSemaphore") == -1) {
      perror("sem_unlink: syncSemaphore\n");
    }
  }
  
  sem_post(clockSem);
  sem_post(timerSem);
  sem_post(syncSem);
  printf("Clock Shutdown complete...\n");
}

/*
void clock_ticSecond() {
  //Abort if sync is in process
  int syncSemVal;
  sem_getvalue(syncSem, &syncSemVal);

  if (syncSemVal == 0) {
    return;
  }

  sem_wait(timerSem);
  sem_wait(clockSem);

  //cancel alarm
  alarm(0);

  //setup next alarm
  alarm(TIMER_RESOLUTION);

  currentTics = ticsPerSecond - 1;
  sem_post(clockSem);
  sem_post(timerSem);

  tic();
}
 */