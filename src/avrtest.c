#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#include "avr.h"

#define AVR_PORT "/dev/ttyUSB1"


int main()
{	
	printf("Starting AVR testing program\n");
	int avr_fd = avr_open(AVR_PORT);
	int speed = 0, RPM = 0;
	
	if(avr_fd <= 0) {
		perror("Unable to connect to the AVR");
		exit(EXIT_FAILURE);
	}
	
	while("forever") 
	{	
		if (speed != 110)
			++speed;
		else
			speed = 0;
		if (RPM != 65500)
			RPM += 100;
		else
			RPM = 0;
			
		send_to_avr(avr_fd, speed, RPM);
	}
	return 0;
}
