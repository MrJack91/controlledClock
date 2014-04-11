/* 
 * File:   main.c
 * Author: Daniel Brun
 *
 * Created on 1. April 2014, 12:59
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <signal.h>

#include "SimpleSocketServer.h"
#include "helper_json.h"
#include "clock.h"


int cleanUpExecuted = 0;

char *server_handle(char *content);

void cleanUp();

int main(int argc, char *argv[]) {

    if (signal (SIGINT, cleanUp) == SIG_IGN){
         signal (SIGINT, SIG_IGN);
    }
    if (signal (SIGKILL, cleanUp) == SIG_IGN){
         signal (SIGKILL, SIG_IGN);
    }
    if (signal (SIGHUP, cleanUp) == SIG_IGN){
         signal (SIGHUP, SIG_IGN);
    }
    if (signal (SIGTERM, cleanUp) == SIG_IGN){
         signal (SIGTERM, SIG_IGN);
    }
    clock_start();
    //TODO: use log4c
    
    /*KeyValuePair kvPairs[2];
    kvPairs[0].key = "Test1";
    kvPairs[0].value = "Value1";
    
    kvPairs[1].key = "Test2";
    kvPairs[1].value = "Value2";
    
    char *myString = json_createString(kvPairs);
    printf("Result: %s\n",myString);
    
    free(myString);
    myString = NULL;*/

    //First startup sequence

    //Pass function pointer to methods...

    //Start DCF77 Reader

    //Start DCF77 Simulator

    //Start analyzer 

    //Start encoder

 //initializeClock();
    //Thread exmplae
   
    pthread_t serverThread;
   

    pthread_create(&serverThread,
            NULL,
            (void *) runServer,
            (void *) server_handle);

    pthread_join(serverThread, NULL);
    //pthread_join(inlineThread, NULL);
    
    
    atexit(cleanUp);
    exit(0);
   
    //!!!!http://www.intel-assembler.it/portale/5/high-precision-timer-clock/source-code-fast-timer-8259-c-asm.asp
    
    //Interrupt Timer: http://www.osdever.net/bkerndev/Docs/pit.htm
    //http://www.inversereality.org/tutorials/interrupt%20programming/timerinterrupt.html
    
    //Interrupts from keyboard:
    //http://www.fysnet.net/demoisr.htm
    //High precision system clock: http://www.delorie.com/gnu/docs/glibc/libc_436.html
    //Hello();
    
    //Assembler wrapper: http://www.techrepublic.com/article/writing-complex-interrupt-handlers-in-c/#.
  
    return (EXIT_SUCCESS);
}

char *server_handle(char *content) {
    printf("Client-Request: %s\n", content);
    
    TimeStruct currTime = clock_getCurrentTime();
    TimeStruct lastSyncTime = clock_getLastSyncTime();
    
    char currTimeStr[26];
    char lastSyncTimeStr[26];
    
    sprintf(currTimeStr,"%04d-%02d-%02dT%02d:%02d:%02d%+2.2d:00",currTime.year,currTime.month,currTime.day,currTime.hour,currTime.minute,currTime.second,currTime.zoneOffset);
    sprintf(lastSyncTimeStr,"%04d-%02d-%02dT%02d:%02d:%02d%+2.2d:00",lastSyncTime.year,lastSyncTime.month,lastSyncTime.day,lastSyncTime.hour,lastSyncTime.minute,lastSyncTime.second,lastSyncTime.zoneOffset);
    
    KeyValuePair kvPairs[2];
    kvPairs[0].key = "CurrentTime";
    kvPairs[0].value = currTimeStr;
    
    kvPairs[1].key = "LastSyncTime";
    kvPairs[1].value = lastSyncTimeStr;
    
    return json_createString(kvPairs);
}

void cleanUp(){
    
    if(cleanUpExecuted != 1){
        cleanUpExecuted = 1;
        printf("Shutdown application...\n");

        shutdownServer();

        printf("Application shutdown finished...exiting now...\n");
    }
    exit(1);
}
