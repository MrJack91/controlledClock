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

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#endif

#include <unistd.h>

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
    server_stop();
    fprintf(stderr, "Failed to bind server socket!");
  }

  //listen to client sockets
  if (listen(serverSocket, MAX_QUEUE) < 0) {
    server_stop();
    fprintf(stderr, "Failed to listen on server socket!");
  }

  unsigned int clntLen;

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
    char *handleResponse;
    socketHandle(NULL, &handleResponse);

    char contentLength[16 + sizeof (int)] = "\0";

    sprintf(contentLength, "Content-Length: %lu\n", strlen(handleResponse));

    if (serverDebugInfo > 0) {
      printf("Sending response...\n");
    }
    sendResponse(clientSocket, "HTTP/1.0 200 OK\n");
    sendResponse(clientSocket, "Connection: close\n");
    sendResponse(clientSocket, "Content-type: text/plain;charset=UTF-8\n");
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
}

static void sendResponse(int socketId, char *content) {
  int length = strlen(content);

  if (send(socketId, content, length, 0) <= (length - 1)) {
    server_stop();
    fprintf(stderr, "Sending failed\n");
  }
}
