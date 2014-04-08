
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

void Hello() {
    printf("Hello");
}

#define RCVBUFSIZE 32
#define MAX_QUEUE 5

#define CHAR_SIZE ((8 * sizeof(int) - 1) / 3 + 2)

char header[] = "HTTP/1.0 200 Ok\nConnection: close\nContent-type: text/html; charset=UTF-8\nstatus:200 OK\nContent-length: ";
char headerEnd[] = "\n\n";
char response[] = "<doctype !html><html><head><title>Bye-bye baby bye-bye</title>"
"<style>body { background-color: #111 }"
"h1 { font-size:4cm; text-align: center; color: black;"
" text-shadow: 0 0 2mm red}</style></head>"
"<body><h1>Goodbye, world!</h1></body></html>\n\n";

int serverSocket = -1;

void runServer(void(*handle)(int)) {
	atexit(shutdownServer);
    //Look at this for Webservers:
    //http://www.paulgriffiths.net/program/c/webserv.php
    //http://www.csd.uoc.gr/~hy556/material/tutorials/cs556-3rd-tutorial.pdf
    
    //HTTP:Header:
    //http://stackoverflow.com/questions/5136165/web-server-problem-in-c
    
    //Create server socket
    serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket < 0) {
        fprintf(stderr, "Failed to create server socket!");
    }

    struct sockaddr_in serverAddr, clientAddr;
	
    memset(&serverAddr, 0, sizeof(serverAddr));
    memset (&(serverAddr.sin_zero), '\0', 8);
	
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //Bind server socket
    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        fprintf(stderr, "Failed to bind server socket!");
    }

    //listen to client sockets
    if (listen(serverSocket, MAX_QUEUE) < 0) {
        fprintf(stderr, "Failed to listen on server socket!");
    }

    unsigned int clntLen;
    
    //accept client sockets
    while (1) {
        clntLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &clntLen);

        puts("Accepted connection...");
        /*char echoBuffer[RCVBUFSIZE];
        int recvMsgSize;
        
        /*if((recvMsgSize = recv(clientSocket,echoBuffer,RCVBUFSIZE,0)) < 0){
             fprintf(stderr, "Failed to read from client socket!");
        }/*
        
        /*while(recvMsgSize > 0){
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
		puts("Sending response to client");
        if(send(clientSocket,"Test",4,0) <= (4-1)){
            fprintf(stderr,"Sending failed\n");
        }
		if(send(clientSocket,"More",4,0) <= (4-1)){
            fprintf(stderr,"Sending failed\n");
        }
		
		//Free memory
        puts("Sent response...free memory");
        //free(deffResp);
		//deffResp = NULL;
        puts("Finished processing");
        close(clientSocket);
        puts("Closed client socket");
    }

}

void shutdownServer(){
	//close server socket
    if (close(serverSocket) == -1) {
        fprintf(stderr, "Failed to close server socket!");
    }
}