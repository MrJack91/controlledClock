
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

#include "SimpleSocketServer.h"

#define RCVBUFSIZE 32
#define MAX_QUEUE 5
#define PORT 7799

#define CHAR_SIZE ((8 * sizeof(int) - 1) / 3 + 2)

int serverSocket = -1;
int on = 1;
/*---------------------------- Internal functions ----------------------------*/
static void sendResponse(int socketId, char *content);

void runServer(char *(*socketHandle)(char*)) {
    atexit(shutdownServer);
    
    //Create server socket
    serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket < 0) {
        fprintf(stderr, "Failed to create server socket!");
    }

    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    
    struct sockaddr_in serverAddr, clientAddr;
	
    memset(&serverAddr, 0, sizeof(serverAddr));
    memset (&(serverAddr.sin_zero), '\0', 8);
	
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //Bind server socket
    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        shutdownServer();
        fprintf(stderr, "Failed to bind server socket!");
    }

    //listen to client sockets
    if (listen(serverSocket, MAX_QUEUE) < 0) {
        shutdownServer();
        fprintf(stderr, "Failed to listen on server socket!");
    }

    unsigned int clntLen;
    
    //accept client sockets
    while (1) {
        clntLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &clntLen);

        if(clientSocket == -1){
            fprintf(stderr, "Couldn't accept Socket!");
            continue;
        }
        
        printf("Accepted connection...");
        /*char readBuffer[RCVBUFSIZE];
        int recvMsgSize;

        recvMsgSize = 1;
        int leCount = 0;
        int lastWasEnd = 0;
        int endOfRequest = 0;
        while(endOfRequest == 0 && recvMsgSize > 0){
            printf("Size: %d",recvMsgSize);
            if((recvMsgSize = recv(clientSocket,readBuffer,RCVBUFSIZE,0)) < 0){
             fprintf(stderr, "Failed to read from client socket!");
            }
            //printf("%s",readBuffer);
            
            int length = strlen(readBuffer);
            int counter = 0;
            
            for(counter = 0;counter < length;counter++){
                printf("%c",readBuffer[counter]);
                if(readBuffer[counter] == '\n'){
                     leCount++;
                     printf("Found one %d %d %d %d",lastWasEnd,leCount,counter,length);
                     lastWasEnd = 1;
                  
                }else{
                    lastWasEnd = 0;
                    leCount = 0;
                }
                //Blank line to indicate end of request...
                if(leCount >= 2 && lastWasEnd){
                    printf("Detected end of request");
                    endOfRequest = 1;
                    break;
                }
            }
            printf("End-Size: %d",recvMsgSize);
        }
        
        printf("Request reading completet...");*/
        
        char *handleResponse = socketHandle(NULL);
        char contentLength[16 + sizeof(int)] = "\0";
        
        sprintf(contentLength,"Content-Length: %d\n",strlen(handleResponse));
        
        printf("Sending response...\n");
        sendResponse(clientSocket,"HTTP/1.0 200 OK\n");
        sendResponse(clientSocket,"Connection: close\n");
        sendResponse(clientSocket,"Content-type: text/plain;charset=UTF-8\n");
        sendResponse(clientSocket,contentLength);
        sendResponse(clientSocket,"expires: -1\n");
        sendResponse(clientSocket,"status: 200\n\n");
        
        sendResponse(clientSocket,handleResponse);
        
        printf("Response sent...\n");
        //TODO: Free fails here...
        // free(handleResponse);
        //handleResponse = NULL;
	
        puts("Finished processing");
        close(clientSocket);
        puts("Closed client socket");
    }

}

void shutdownServer(){
    printf("Shutdown Server...\n");
    //close server socket
    if(serverSocket != -1){
        if (close(serverSocket) == -1) {
            fprintf(stderr, "Failed to close server socket!\n");
        }else{
            printf("Server-Socket closed...\n");
            serverSocket = -1;
        }
    }
    printf("Server Shutdown complete...\n");
}
static void sendResponse(int socketId, char *content){
    int length = strlen(content);
    
    if(send(socketId,content,length,0) <= (length-1)){
        shutdownServer();
        fprintf(stderr,"Sending failed\n");
    }
}
