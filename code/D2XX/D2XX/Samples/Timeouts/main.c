/*
	A very simple example to show a 5 second timout when reading nothing from a
	device. This example assumes that there is no hardware attached to the device
	so a timeout will occur after 5 seconds

	To build use the following gcc statement
	(assuming you have the d2xx library in the /usr/local/lib directory).
	gcc -o timeouts main.c -L. -lftd2xx -Wl,-rpath /usr/local/lib
	gcc -o timeouts main.c -L. -lftd2xx -Wl,-rpath /Users/michael/projects/controlledClock/code/D2XX/D2XX/
*/

#include <stdio.h>
#include "../ftd2xx.h"

#define BUF_SIZE 0x1000

#define MAX_DEVICES		4

int main(int argc, char *argv[])
{
	char cBufRead[BUF_SIZE];
	DWORD dwBytesRead;
	FT_STATUS ftStatus;
	FT_HANDLE ftHandle;
	int iport;
	int i;

	if(argc > 1) {
		sscanf(argv[1], "%d", &iport);
	}
	else {
		iport = 0;
	}

	FT_SetVIDPID(1027, 59530); // use our VID and PID
	ftStatus = FT_Open(iport, &ftHandle);
	if(ftStatus != FT_OK) {
		/*
			This can fail if the ftdi_sio driver is loaded
		 	use lsmod to check this and rmmod ftdi_sio to remove
			also rmmod usbserial
		 */
		printf("FT_Open(%d) failed, with error %d.\n", iport, (int)ftStatus);
		printf("Use lsmod to check if ftdi_sio (and usbserial) are present.\n");
		printf("If so, unload them using rmmod, as they conflict with ftd2xx.\n");
		return 1;
	}

	FT_SetTimeouts(ftHandle, 5, 5);	// 3 second read timeout

	for(i = 0; i < 10000; i++) {
		FT_Read(ftHandle, cBufRead, BUF_SIZE, &dwBytesRead);

		if ((int)dwBytesRead != 0) {
			printf("dwBytesRead: %d\n", (int)dwBytesRead);
		}

		/*
		if(dwBytesRead != BUF_SIZE) {
			printf("Timeout %d\n", i);
		} else {
			printf("Read %d\n", (int)dwBytesRead);
		}
		*/
	}


	FT_Close(ftHandle);
	return 0;
}
