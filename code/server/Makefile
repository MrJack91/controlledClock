CC = gcc
CFLAGS = -Wall
CFLAGS_WIN = -Wall -m32 -DWIN
CLIBS_WIN = -L. -L"/cygdrive/c/Cygwin86/usr/local/lib/ftd2xx-dyn-32/" -lpthread -lftd2xx 
CFLAGS_OSX = -Wall -DOSX


# full programm on osx
fullOsx: main.o SimpleSocketServer.o helper_json.o Clock.o dcf77_reader.o dcf77_decoder.o
	$(CC) $(CFLAGS) -o controlledClock.exe main.o SimpleSocketServer.o helper_json.o Clock.o dcf77_reader.o dcf77_decoder.o -lpthread -L. -lftd2xx

# full programm on win
fullWin: main.o SimpleSocketServer.o helper_json.o Clock.o dcf77_reader.o dcf77_decoder.o
	$(CC) $(CFLAGS) -o controlledClock.exe -DWIN main.o SimpleSocketServer.o helper_json.o Clock.o dcf77_reader.o dcf77_decoder.o $(CLIBS_WIN)

# server main
server: main.o SimpleSocketServer.o helper_json.o Clock.o
	$(CC) $(CFLAGS) -o controlledClock.exe main.o SimpleSocketServer.o helper_json.o Clock.o -lpthread 
	
SimpleSocketServer.o: SimpleSocketServer.c SimpleSocketServer.h
	$(CC) $(CFLAGS) -c SimpleSocketServer.c SimpleSocketServer.h

helper_json.o: helper_json.c helper_json.h
	$(CC) $(CFLAGS) -c helper_json.c helper_json.h

Clock.o: Clock.c Clock.h
	$(CC) $(CFLAGS) -c Clock.c Clock.h

dcf77_reader.o: dcf77_reader.c dcf77_reader.h	
	$(CC) $(CFLAGS) -c dcf77_reader.c -DWIN -o dcf77_reader.o

dcf77_decoder.o: dcf77_decoder.c dcf77_decoder.h
	$(CC) $(CFLAGS) -c dcf77_decoder.c -o dcf77_decoder.o
	
# only read the data from the usb device (for debuggin)
reader_standalone_osx: dcf77_reader_standalone.c
	$(CC) $(CFLAGS) -o dcf77_reader_standalone.out dcf77_reader_standalone.c -L. -lftd2xx

reader_standalone_win: dcf77_reader_standalone.c
	$(CC) $(CFLAGS) -o controlledClock.exe dcf77_reader_standalone.c $(CLIBS_WIN)

fullOsx: CFLAGS = $(CFLAGS_OSX)

fullWin: CFLAGS = $(CFLAGS_WIN)
	
reader_standalone_osx: CFLAGS = $(CFLAGS_OSX)

reader_standalone_win: CFLAGS = $(CFLAGS_WIN)
	
# stuff
clean:
	rm -f *.o
	rm -f *.out
	rm -f *.exe
	rm -f *.h.gch
