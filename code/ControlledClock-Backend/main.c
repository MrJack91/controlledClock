/* 
 * File:   main.c
 * Author: Daniel Brun
 *
 * Created on 1. April 2014, 12:59
 */

#include <stdio.h>
#include <stdlib.h>
#include "SimpleSocketServer.h"
#include <pthread.h>

#include <signal.h>
#include <unistd.h>

/*
 * 
 */
typedef enum {
    false, true
} bool;

void handle(int aNumber);

void InterruptSignalHandler(int signalType);

int main(int argc, char *argv[]) {
    //TODO: use log4c


    //First startup sequence

    //Pass function pointer to methods...

    //Start DCF77 Reader

    //Start DCF77 Simulator

    //Start analyzer 

    //Start encoder

    //Semaphores:
    //http://www.daniweb.com/software-development/c/threads/330806/semaphores-in-c

    //Function Pointers -> Callback
    //http://www.cprogramming.com/tutorial/function-pointers.html

    //http://www.chemie.fu-berlin.de/chemnet/use/info/libc/libc_21.html


    //Thread exmplae
    int serverParam = 1;
    pthread_t serverThread;
    pthread_t inlineThread;

    pthread_create(&serverThread,
            NULL,
            (void *) runServer,
            (void *) handle);

    pthread_create(&inlineThread,
            NULL,
            (void *) Hello,
            (void *) NULL);

    //pthread_join(serverThread, NULL);
    //pthread_join(inlineThread, NULL);

    struct sigaction handler;
    /* Signal handler specification structure */
    handler.sa_handler =  InterruptSignalHandler;
    /* Set handler function */
    if (sigfillset(&handler.sa_mask) < 0)
        /* Create mask that masks all signals */
        printf("sigfillset() failed");
        handler.sa_flags = 0;
    if (sigaction(12, &handler, 0) < 0)
        /* Set signal handling for interrupt signals */
        printf("sigaction() failed");
    for (;;) pause();
    /* Suspend program until signal received */
    exit(0);

    
    //Interrupt Timer: http://www.osdever.net/bkerndev/Docs/pit.htm
    //http://www.inversereality.org/tutorials/interrupt%20programming/timerinterrupt.html
    
    //Interrupts from keyboard:
    //http://www.fysnet.net/demoisr.htm
    //High precision system clock: http://www.delorie.com/gnu/docs/glibc/libc_436.html
    //Hello();
    return (EXIT_SUCCESS);
}

void handle(int aNumber) {

    printf("Handle was %d\n", aNumber);
}

void InterruptSignalHandler (int signalType) {
printf ("Interrupt received. Exiting program.\n");
exit(1);
}