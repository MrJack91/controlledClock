CC = gcc
CFLAGS = -Wall
DEPS = SimpleSocketServer.o SimpleSocketServer.h
OBJ = main.o SimpleSocketServer.h

all: main.o SimpleSocketServer.o helper_json.o clock.o
	gcc -o controlledClock main.o SimpleSocketServer.o helper_json.o clock.o -lpthread 
	
SimpleSocketServer.o: SimpleSocketServer.c SimpleSocketServer.h
	gcc -c SimpleSocketServer.c SimpleSocketServer.h
	
helper_json.o: helper_json.c helper_json.h
	gcc -c helper_json.c helper_json.h
	
clock.o: clock.c clock.h
	gcc -c clock.c clock.h

clean:
	rm -f *.o
	rm -f controlledClock