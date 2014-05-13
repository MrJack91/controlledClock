CC = gcc
CFLAGS = -Wall 

all: 
	# for build:
	# full: use "make full"
	# dcf77: use "make osx" | "make win"
	# server: use "make server"
# gcc -o dcf77.out dcf77_main.o dcf77_reader.o dcf77_decoder.o -lpthread -L. -lftd2xx
# gcc -o dcf77_reader dcf77_reader.c -L. -lftd2xx -Wl,-rpath /cygdrive/d/Development/git_workspaces/controlledClock/code/D2XX/bin && ./dcf77_reader
# gcc -o dcf77.out dcf77_main.o dcf77_reader.o dcf77_decoder.o -lpthread -L"/cygdrive/c/Program Files/Cygwin64/usr/local/lib/ftd2xx" -lftd2xx

osx: dcf77_main.o dcf77_reader.o dcf77_decoder.o
	gcc -o dcf77.out dcf77_main.o dcf77_reader.o dcf77_decoder.o -lpthread -L. -lftd2xx
	
win: dcf77_main.o dcf77_reader.o dcf77_decoder.o
	gcc -m64 -o dcf77_reader.exe  dcf77_main.o dcf77_reader.o dcf77_decoder.o -L. -lpthread "/cygdrive/c/Cygwin64/usr/local/lib/ftd2xx/ftd2xx.lib"
	# gcc -m64 -o dcf77_reader.exe  dcf77_main.o dcf77_reader.o dcf77_decoder.o -L. -L"/cygdrive/c/Program Files/Cygwin64/usr/local/lib/ftd2xx" -lpthread -lftd2xx
	# gcc -o dcf77_reader dcf77_reader.c -L. -L"/cygdrive/c/Program Files/Cygwin64/usr/local/lib/ftd2xx" -lftd2xx -lpthread -Wl,-rpath "/cygdrive/c/Program Files/Cygwin64/usr/local/lib/ftd2xx"
	# Somethin on dynamic / shared libs: http://www.cprogramming.com/tutorial/shared-libraries-linux-gcc.html && http://www.geeksforgeeks.org/static-vs-dynamic-libraries/

# full programm on win
fullWin: main.o SimpleSocketServer.o helper_json.o clock.o dcf77_reader.o dcf77_decoder.o
	gcc -Wall -m32 -mcmodel=large -o controlledClock.exe main.o SimpleSocketServer.o helper_json.o clock.o dcf77_reader.o -L. -lpthread -L"/cygdrive/c/Cygwin86/usr/local/lib/ftd2xx-dyn-32/" -lftd2xx dcf77_decoder.o

# full programm
full: main.o SimpleSocketServer.o helper_json.o clock.o dcf77_reader.o dcf77_decoder.o
	gcc -o controlledClock.exe main.o SimpleSocketServer.o helper_json.o clock.o dcf77_reader.o dcf77_decoder.o -lpthread -L. -lftd2xx

# server main
server: main.o SimpleSocketServer.o helper_json.o clock.o
	gcc -o controlledClock.exe main.o SimpleSocketServer.o helper_json.o clock.o -lpthread 
	
SimpleSocketServer.o: SimpleSocketServer.c SimpleSocketServer.h
	gcc -c SimpleSocketServer.c SimpleSocketServer.h

helper_json.o: helper_json.c helper_json.h
	gcc -c helper_json.c helper_json.h

clock.o: clock.c clock.h
	gcc -c clock.c clock.h

# dcf77 main
#osx: dcf77_main.o dcf77_reader.o dcf77_decoder.o
#	gcc -o dcf77.exe dcf77_main.o dcf77_reader.o dcf77_decoder.o -lpthread -L. -lftd2xx
	
#win: dcf77_main.o dcf77_reader.o dcf77_decoder.o
#	gcc -o dcf77_reader dcf77_reader.c -L"/cygdrive/c/Program Files/Cygwin64/usr/local/lib/ftd2xx/" -lftd2xx -Wl,-rpath /cygdrive/d/Development/git_workspaces/controlledClock/code/D2XX/bin && ./dcf77_reader
	
dcf77_reader.o: dcf77_reader.c dcf77_reader.h	
	gcc -c dcf77_reader.c -o dcf77_reader.o

dcf77_decoder.o: dcf77_decoder.c dcf77_decoder.h
	gcc -c dcf77_decoder.c -o dcf77_decoder.o

# only read the data from the usb device (for debuggin)
reader_standalone: dcf77_reader_standalone.c
	gcc -o dcf77_reader_standalone.out dcf77_reader_standalone.c -L. -lftd2xx

reader_standalone_win: dcf77_reader_standalone.c
	gcc -Wall -m64 -mcmodel=large -o controlledClock.exe dcf77_reader_standalone.c -L. -L"/cygdrive/c/Cygwin64/usr/local/lib/ftd2xx-dyn-64" -lftd2xx 

# stuff
clean:
	rm -f *.o
	rm -f *.out
	rm -f *.exe
	rm -f *.h.gch
