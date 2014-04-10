
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
#define PORT 9999

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
        char *handleResponse = socketHandle(NULL);

        printf("Sending response...\n");
        sendResponse(clientSocket,"HTTP/1.0 200 OK\n");
        sendResponse(clientSocket,"Connection: close\n");
        sendResponse(clientSocket,"Content-type: text/plain;charset=UTF-8\n");
        sendResponse(clientSocket,"expires: -1\n");
        sendResponse(clientSocket,"status: 200\n\n");
        sendResponse(clientSocket,handleResponse);
        sendResponse(clientSocket,"\n\n");
        
        printf("Response sent...\n");
        //TODO: Free fails here...
        // free(handleResponse);
        //handleResponse = NULL;
       // free(handleResponse);
        //handleResponse = NULL;
        //TODO: Implement read
        /*int recvMsgSize;
        
        if((recvMsgSize = recv(clientSocket,echoBuffer,RCVBUFSIZE,0)) < 0){
             fprintf(stderr, "Failed to read from client socket!");
        }
        
        while(recvMsgSize > 0){
            //puts("Sending back to client...");
            if(send(clientSocket,echoBuffer,recvMsgSize,0) != recvMsgSize){
                 fprintf(stderr, "Failed to write to client socket!");
            }
            printf("Reding input...%d\n",recvMsgSize);
            if((recvMsgSize = recv(clientSocket,echoBuffer,RCVBUFSIZE,0)) < 0){
             fprintf(stderr, "Failed to read from client socket!");
            }
            printf("Reding input 2...%d\n",recvMsgSize);
            printf("Read: %s\n",echoBuffer);
        }*/
	//Identify length of response
        /*char resp_len[CHAR_SIZE];
        sprintf(resp_len, "%d", strlen(response));
		
		puts("Calculating size");
		//Calculate length of message
        size_t message_len = strlen(header) + strlen(response) + strlen(resp_len) + strlen(headerEnd) + 1;
		
		printf("Creating output string %d %d %d %d %d",message_len,strlen(header) , strlen(response) , strlen(resp_len) , strlen(headerEnd));
		//Create output string
        char deffResp[message_len + 1];
		puts("Allocated");
		stpcpy(deffResp,header);
			puts("1");
        stpcpy(deffResp,resp_len);
			puts("2");
        stpcpy(deffResp,headerEnd);
			puts("3");
        stpcpy(deffResp,response);
			puts("4");*/

		//Send response to client
        /*sendResponse(clientSocket,"Test");
		puts("Sending response to client");
        if(send(clientSocket,"Test",4,0) <= (4-1)){
            fprintf(stderr,"Sending failed\n");
        }
	if(send(clientSocket,"More",4,0) <= (4-1)){
            fprintf(stderr,"Sending failed\n");
        }*/
		
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
