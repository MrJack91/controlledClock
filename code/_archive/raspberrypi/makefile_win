CC = gcc
CFLAGS = -Wall
DEPS = SimpleSocketServer.o SimpleSocketServer.h
OBJ = main.o SimpleSocketServer.h

all: main.o SimpleSocketServer.o
	gcc -o controlledClock main.o SimpleSocketServer.o -lpthread 
	
main.o:	main.c SimpleSocketServer.h
	gcc -c main.c SimpleSocketServer.h -lpthread 
	
SimpleSocketServer.o: SimpleSocketServer.c SimpleSocketServer.h
	gcc -c SimpleSocketServer.c SimpleSocketServer.h

clean:
	rm -f *.o
	rm -f controlledClock