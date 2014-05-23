/* 
 * File:    SimpleSocketServer.c
 * Author:  Daniel Brun
 * Created: 05.04.2014
 * 
 * Description: 
 * Provides a simple socket server implementation which sends a request given 
 * by a handle back to the client.
 *    
 */

/*---------------------------- Includes: System ------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#endif

#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/*---------------------------- Includes: User-Libs ---------------------------*/
#include "SimpleSocketServer.h"

/*---------------------------- Declarations ----------------------------------*/
#define RCVBUFSIZE 32
#define MAX_QUEUE 5
#define PORT 7899

#define CHAR_SIZE ((8 * sizeof(int) - 1) / 3 + 2)

int serverSocket = -1;
int on = 1;
int serverDebugInfo = 0;

// define how long the while for servers will be running
int acceptSockts = 0;

/*---------------------------- Internal functions ----------------------------*/

/**
 * Sends a response to the client. If the send process was not successfully,
 * an error thrown.
 * 
 * @param socketId The underlying socket where the content msut be sent.
 * @param content The content to send.
 */
static void sendResponse(int socketId, char *content);

/*---------------------------- Implementations -------------------------------*/

void server_start(void (*socketHandle)(char*, char**)) {
  atexit(server_stop);

  //Create server socket
  serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (serverSocket < 0) {
     fprintf(stderr, "Failed to create server socket!");
     server_stop();
  }

  //Set socket options
  setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (int));

  struct sockaddr_in serverAddr, clientAddr;

  memset(&serverAddr, 0, sizeof (serverAddr));
  memset(&(serverAddr.sin_zero), '\0', 8);

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  //Bind server socket
  if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof (serverAddr)) < 0) {
    fprintf(stderr, "Failed to bind server socket!");
    server_stop();
  }

  //listen to client sockets
  if (listen(serverSocket, MAX_QUEUE) < 0) {
    fprintf(stderr, "Failed to listen on server socket!");
    server_stop();
  }

  printf("Server startup complete!\n");
  socklen_t clntLen;

  acceptSockts = 1;
  //accept client sockets (second part, stop if serversocket is not working -> avoid many outputs on unix)
  while (1 && acceptSockts == 1) {
      
    clntLen = sizeof (clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &clntLen);

    if (clientSocket == -1) {
      fprintf(stderr, "Couldn't accept Socket!\n");
      continue;
    }

    if (serverDebugInfo > 0) {
      printf("Accepted connection...\n");
    }
    
    fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL) | O_NONBLOCK);
    
    //Reading request
    char readBuffer[RCVBUFSIZE];
    int recvMsgSize;

    recvMsgSize = 1;
    int leCount = 0;
    int lastWasEnd = 0;
    int endOfRequest = 0;
    
    while(endOfRequest == 0 && recvMsgSize > 0){
        if (serverDebugInfo > 0) {
            printf("Size: %d\n",recvMsgSize);
        }
        if((recvMsgSize = recv(clientSocket,readBuffer,RCVBUFSIZE,0)) < 0 && serverDebugInfo > 0){
         fprintf(stderr, "Failed to read from client socket!\n");
        }
        
        if (serverDebugInfo > 0) {
            printf("Request: %s\nProcessing...:\n",readBuffer);
        }
        int length = strlen(readBuffer);
        int counter = 0;

        for(counter = 0;counter < length;counter++){
            if (serverDebugInfo > 0) {
                printf("%c",readBuffer[counter]);
            }
            if(readBuffer[counter] == '\n'){
                 leCount++;
                 if (serverDebugInfo > 0) {
                    printf("Found one %d %d %d %d\n",lastWasEnd,leCount,counter,length);
                 }
                 lastWasEnd = 1;

            }else{
                lastWasEnd = 0;
                leCount = 0;
            }
            
            //Blank line to indicate end of request...
            if(leCount >= 2 && lastWasEnd){
                if (serverDebugInfo > 0) {
                    printf("Detected end of request\n");
                }
                endOfRequest = 1;
                break;
            }
        }
        if (serverDebugInfo > 0) {
            printf("End-Size: %d\n",recvMsgSize);
        }
    }
    if (serverDebugInfo > 0) {
        printf("Request reading completet...\n");
    }
    
    char *handleResponse;
    socketHandle(NULL, &handleResponse);

    char contentLength[16 + sizeof (int)] = "\0";

    sprintf(contentLength, "Content-Length: %zu\n", strlen(handleResponse));

    if (serverDebugInfo > 0) {
      printf("Sending response...\n");
    }
    sendResponse(clientSocket, "HTTP/1.0 200 OK\n");
    sendResponse(clientSocket, "Connection: close\n");
    sendResponse(clientSocket, "Content-type: text/plain;charset=UTF-8\n");
    sendResponse(clientSocket, "Access-Control-Allow-Headers: *\n");
    sendResponse(clientSocket, "Access-Control-Allow-Origin: *\n");
    sendResponse(clientSocket, contentLength);
    sendResponse(clientSocket, "expires: -1\n");
    sendResponse(clientSocket, "status: 200\n\n");

    sendResponse(clientSocket, handleResponse);

    if (serverDebugInfo > 0) {
      printf("Response sent...\n");
    }
    //No free needed here.
    free(handleResponse);
    handleResponse = NULL;

    if (serverDebugInfo > 0) {
      puts("Finished processing");
    }
    close(clientSocket);
    if (serverDebugInfo > 0) {
      puts("Closed client socket");
    }
  }

}

void server_stop() {
  // stop accept sockets in server-while
  acceptSockts = 0;
  printf("Shutdown Server... (Server Socket ID: %d)\n", serverSocket);
  //close server socket
  if (serverSocket != -1) {
    if (close(serverSocket) == -1) {
      fprintf(stderr, "Failed to close server socket!\n");
    } else {
      printf("Server-Socket closed...\n");
      serverSocket = -1;
    }
  }
  printf("Server Shutdown complete...\n");
  
  pthread_exit((void*)0);
}

static void sendResponse(int socketId, char *content) {
  int length = strlen(content);

  if (send(socketId, content, length, 0) <= (length - 1)) {
    server_stop();
    fprintf(stderr, "Sending failed\n");
  }
}
