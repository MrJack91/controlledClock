/* 
 * File:   SimpleSocketServer.h
 * Author: Daniel Brun
 *
 * Created on 1. April 2014, 13:03
 */

#ifndef SIMPLESOCKETSERVER_H
#define	SIMPLESOCKETSERVER_H

#ifdef	__cplusplus
extern "C" {
#endif

void Hello();

void runServer( void(*handle)(int));


#ifdef	__cplusplus
}
#endif

#endif	/* SIMPLESOCKETSERVER_H */
