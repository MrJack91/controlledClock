/* 
 * File:    helper_json.c
 * Author:  Daniel Brun
 * Created: 09.04.2014
 * 
 * Description: 
 * Provides some JSON helper methods.
 *  
 */

/*---------------------------- Includes: System ------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------------------------- Includes: User-Libs ---------------------------*/
#include "helper_json.h"

char * json_createString(KeyValuePair *keyValuePairs){
    int size = 0; //empty json string

    //Calculate size
    int arrSize = sizeof(KeyValuePair) / sizeof(keyValuePairs);

    if (arrSize > 0){
        //minimal size is known
        size = 3;
        
        //Summ up the different lengths of the elments.
        int elem = 0;
        for(elem = 0; elem < arrSize;elem++){
            size = size + strlen(keyValuePairs[elem].key) + strlen(keyValuePairs[elem].value) + 5;
            
            if(elem < (arrSize -1)){
                size++;
            }
        }
        
        //Allocate memory
        char *jsonString = malloc(size*sizeof(*jsonString));

        if(jsonString == NULL){
                 fprintf(stderr, "Failed to allocate memory for json conversion!\n");
                 exit(2);
        }
        
        //Create the json string.
        strncpy(jsonString,"{",2);
        for(elem = 0; elem < arrSize;elem++){
            strncat(jsonString,"\"",1);
            strncat(jsonString,keyValuePairs[elem].key,strlen(keyValuePairs[elem].key));
            strncat(jsonString,"\":\"",3);
             
            strncat(jsonString,keyValuePairs[elem].value,strlen(keyValuePairs[elem].value));
            strncat(jsonString,"\"",1);
            if(elem < (arrSize - 1)){
                strncat(jsonString,",",1);//Comma needed at the end
            }
         }

        strncat(jsonString,"}",1);
        strncat(jsonString,"\0",1);

        return jsonString;
    }
     return "";
}
