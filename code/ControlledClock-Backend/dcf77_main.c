#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h> // to use the pow command

#include "dcf77_reader.h"
#include "dcf77_decoder.h"


int main() {
  printf("========================================================================================================================================================================\n");
  printf("==\tSTART dcf77_main\t==\n");
  printf("========================================================================================================================================================================\n");
  
  // decode manually for debugging
  // dcf77_decode();
  
  // read data (will call decoder if data are available)
  dcf77_read();
  
  printf("==\tEND dcf77_main\t\t==\n");
}