
/* 
 * File:   helper_json.c
 * Author: Daniel Brun
 *
 * Created on 9. April 2014, 17:12
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helper_json.h"

char * json_createString(KeyValuePair *keyValuePairs){
    int size = 0; //empty json string

    int arrSize = sizeof(KeyValuePair) / sizeof(keyValuePairs);

    if (arrSize > 0){
        size = 2 + arrSize + 1;
        
        int elem = 0;
        for(elem = 0; elem < arrSize;elem++){
            size = size + strlen(keyValuePairs[elem].key) + strlen(keyValuePairs[elem].value) + 5;
        }
        
        char *jsonString = malloc(size*sizeof(*jsonString));
        
        if(jsonString == NULL){
                 fprintf(stderr, "Failed to allocate memory for json conversion!\n");
                 exit(2);
        }
        
        strncpy(jsonString,"{",1);
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
        strncat(jsonString,"}\0",2);
        return jsonString;
    }
     return "";
}
