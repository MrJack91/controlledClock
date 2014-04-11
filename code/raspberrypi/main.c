/* 
 * File:    main.c
 * Author:  Daniel Brun
 * Created: 01.04.2014
 * 
 * Description: 
 * Main programm of the 'ControlledClock'. Starts and coordinates the different
 * modules.
 *  
 */

/*---------------------------- Includes: System ------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <signal.h>

/*---------------------------- Includes: User-Libs ---------------------------*/
#include "SimpleSocketServer.h"
#include "helper_json.h"
#include "clock.h"

/*---------------------------- Declarations ----------------------------------*/
int cleanUpExecuted = 0;

/*---------------------------- Internal functions ----------------------------*/

/**
 * Handles the http request from the web server. A json string with the current
 * time from the clock module und the last sync time will be sent back to the
 * client.
 * 
 * @param content The request content.
 * @return The content to be sent to the client.
 */
char *server_handle(char *content);

/**
 * Shuts all modules properly down and exits.
 */
void main_exit();

int main(int argc, char *argv[]) {

    //Setup signal handlers for unexcpected termination
    if (signal (SIGINT, main_exit) == SIG_IGN){
         signal (SIGINT, SIG_IGN);
    }
    if (signal (SIGKILL, main_exit) == SIG_IGN){
         signal (SIGKILL, SIG_IGN);
    }
    if (signal (SIGHUP, main_exit) == SIG_IGN){
         signal (SIGHUP, SIG_IGN);
    }
    if (signal (SIGTERM, main_exit) == SIG_IGN){
         signal (SIGTERM, SIG_IGN);
    }
    
    clock_start();

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
            (void *) server_start,
            (void *) server_handle);

    pthread_join(serverThread, NULL);
    //pthread_join(inlineThread, NULL);
    
    
    atexit(main_exit);
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

void main_exit(){
    
    if(cleanUpExecuted != 1){
        cleanUpExecuted = 1;
        printf("Shutdown application...\n");

        server_stop();

        printf("Application shutdown finished...exiting now...\n");
    }
    exit(1);
}
