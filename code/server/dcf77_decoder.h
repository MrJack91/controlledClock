/* 
 * File:   dcf77_decoder.h
 * Author: michael
 *
 * Created on 10. April 2014, 23:21
 */

#ifndef DCF77_DECODER_H
#define	DCF77_DECODER_H

#ifdef	__cplusplus
extern "C" {
#endif

void dcf77_decode();
  
void addReceivedByte(unsigned int received);



#ifdef	__cplusplus
}
#endif

#endif	/* DCF77_DECODER_H */

