/* 
 * File:   clock.h
 * Author: Daniel Brun
 *
 * Created on 10. April 2014, 19:54
 */

#ifndef CLOCK_H
#define	CLOCK_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
        int zoneOffset;
    } TimeStruct;

    void clock_start();
    
    void clock_shutdown();

    TimeStruct clock_getCurrentTime();
    
    TimeStruct clock_getLastSyncTime();
    
    void clock_syncTime(TimeStruct aTime);
    
    TimeStruct subtractSeconds(TimeStruct date, int seconds);
    
    void clock_ticSecond();
#ifdef	__cplusplus
}
#endif

#endif	/* CLOCK_H */

