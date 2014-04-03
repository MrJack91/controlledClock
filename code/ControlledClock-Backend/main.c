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
/*
 * 
 */
typedef enum { false, true } bool;

void handle(int aNumber);

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
    
    pthread_join(serverThread, NULL);
    pthread_join(inlineThread, NULL);
    
    //Hello();
    return (EXIT_SUCCESS);
}

void handle(int aNumber){
    
    printf("Handle was %d\n",aNumber);
}
