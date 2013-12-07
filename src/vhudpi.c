#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#include "avr.h"
#include "obd.h"

#define OBD_PORT "/dev/ttyUSB2"  // for the USB ELM327
//#define OBD_PORT "/dev/rfcomm0"  // for the Bluetooth ELM327
//#define OBD_PORT "/dev/ttyS3"	// for Cygwin on Windows

#define AVR_PORT "/dev/ttyUSB1"

#define TIMING_LOG "timing.txt"

void obd_setup(int fd)
{
	printf("Resetting the chip...");

	if (obd_reset(fd))
		printf("Success\n");
	else
		printf("FAILURE\n");

	//turn off echos
	obd_wait_until_echo(fd, false);

	//wait until the chip is working again
	printf("Making sure chip is working...");
	obd_wait_until_on(fd);
	printf("Success\n");

	//set the read to blocking
	set_blocking(fd, 1);

	//ready to go
	printf("OBD is ready\n");
}

FILE *open_timing_log() {
	return fopen(TIMING_LOG, "w");
}

void log_time(FILE *fp, time_t request_time) {
	fprintf(fp, "%d\n", (int) request_time);
}

int main()
{
	int fd = obd_open(OBD_PORT);
	int avr_fd = avr_open(AVR_PORT);
	FILE* timelog = open_timing_log();

	int RPM = 0;
	int speed = 0;
	
	time_t start, end;

	obd_setup(fd);

	printf("Beginning read cycle...\n");

	while (true) {
		
		start = time(0);
		RPM = get_rpm(fd);
		end = time(0);
		log_time(timelog, end - start);
		
		speed = get_speed(fd);

		if ((RPM != -1) && (speed != -1)) {
			send_to_avr(avr_fd, speed, RPM);
			printf("RPM: %d\nSpeed: %d\n", RPM, speed);
		}
		//flush buffers just in case
		tcflush(fd, TCIOFLUSH);
	}

	return 0;
}
