/* 
 * File:   dcf77_main.c
 * Author: Michael Hadorn
 * Created: 10.04.2014
 * 
 * Description: 
 * Manager of the whole programm.
 *  
 */

/*---------------------------- Includes: System ------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*---------------------------- Includes: User-Libs ---------------------------*/
#include "dcf77_reader.h"
#include "dcf77_decoder.h"

/*---------------------------- Implementations -------------------------------*/
int main() {
  printf("========================================================================================================================================================================\n");
  printf("==\tSTART dcf77_main\t==\n");
  printf("========================================================================================================================================================================\n");
  
  // read saved values from device manually for debugging decoder
  // dcf77_decode();
  
  // read data direct from usb (will call decoder always if data are available)
  dcf77_read();
  
  printf("==\tEND dcf77_main\t\t==\n");
}