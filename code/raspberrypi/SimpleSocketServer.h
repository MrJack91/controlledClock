/* 
 * File:    SimpleSocketServer.h
 * Author:  Daniel Brun
 * Created: 05.04.2014
 * 
 * Description: 
 * Provides a simple socket server implementation which sends a request given 
 * by a handle back to the client.
 *  
 */

#ifndef SIMPLESOCKETSERVER_H
#define	SIMPLESOCKETSERVER_H

#ifdef	__cplusplus
extern "C" {
#endif

/*---------------------------- Functions -------------------------------------*/
    
/**
 * Starts the server on the preconfigred port.
 * 
 * @param handle    The function which should be processed after a connection 
 *                  is established.
 */
void server_start( char *(*handle)(char*));

/**
 * Stops the server.
 */
void server_stop();

#ifdef	__cplusplus
}
#endif

#endif	/* SIMPLESOCKETSERVER_H */

