CC = gcc
CFLAGS = -Wall

all: dcf77_main.o dcf77_reader.o dcf77_decoder.o
	gcc -o dcf77.out dcf77_main.o dcf77_reader.o dcf77_decoder.o -lpthread -L. -lftd2xx

#dcf77_main.o: dcf77_main.c
#	gcc -c dcf77_main.c -o dcf77_main.o

dcf77_reader.o: dcf77_reader.c dcf77_reader.h
	gcc -c dcf77_reader.c -o dcf77_reader.o
#	gcc -o dcf77_reader dcf77_reader.c -L. -lftd2xx

dcf77_decoder.o: dcf77_decoder.c dcf77_decoder.h
	gcc -c dcf77_decoder.c -o dcf77_decoder.o

clean:
	rm -f *.o
	rm -f dcf77.out
