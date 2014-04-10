/* 
 * File:   helper_json.h
 * Author: Daniel Brun
 *
 * Created on 9. April 2014, 17:12
 */

#ifndef HELPER_JSON_H
#define	HELPER_JSON_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct {
        char *key;
        char *value;
    } KeyValuePair;

/**
 * Creates a json string from the given key value pairs.
 * 
 * @param keyValuePairs The key value pairs
 * @return a pointer to the json string. (free is required)
 */
char * json_createString(KeyValuePair *keyValuePairs);

#ifdef	__cplusplus
}
#endif

#endif	/* HELPER_JSON_H */

