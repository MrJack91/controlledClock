/* 
 * File:   Clock.c
 * Author: Daniel Brun
 *
 * Created on 5. April 2014, 12:26
 */

#include <stdio.h>
#include <stdlib.h>

#include "Clock.h"
#include <signal.h>

void SignalHandler(int signal)
{
    printf("Signal: %d", signal);
    if (signal == SIGABRT) {
        // abort signal handler code
    } else {
        // ...
    }
}

void initializeClock(){
    
    //Perhaps the new way: http://msdn.microsoft.com/en-us/library/xdkz3x12.aspx
    
    
    //http://www.mycfiles.com/2011/10/interrupt-programming-in-c.html
    //http://cboard.cprogramming.com/c-programming/141234-need-pointer-help-interrupt-vectors.html
    
    typedef void (*SignalHandlerPointer)(int);
 
    SignalHandlerPointer previousHandler;
    previousHandler = signal(SIGABRT, SignalHandler);
 
    abort();
}

// SOURCE: http://www.intel-assembler.it/portale/5/high-precision-timer-clock/source-code-fast-timer-8259-c-asm.asp
