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

#define AVR_PORT "/dev/ttyUSB1"

struct AVRPacket
{
	uint16_t header;
	uint8_t	speed;
	uint16_t rpm;
}__attribute((packed))__;

int set_interface_attribs(int fd, int speed, int parity)
{
	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if (tcgetattr(fd, &tty) != 0) {
		printf("error %s from tcgetattr", strerror(errno));
		return -1;
	}

	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);

	tty.c_cflag &= ~CSIZE;	/* Mask the character size bits */
	tty.c_cflag |= CS8;	/* Select 8 data bits */
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;	// ignore break signal
	tty.c_lflag = 0;	// no signaling chars, no echo,
	// no canonical processing
	tty.c_oflag = 0;	// no remapping, no delays
	tty.c_cc[VMIN] = 0;	// read doesn't block
	tty.c_cc[VTIME] = 5;	// 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY);	// shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);	// ignore modem controls,
	// enable reading
	tty.c_cflag &= ~PARENB;	// shut off parity
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("error %s from tcsetattr", strerror(errno));
		return -1;
	}
	return 0;
}

void set_blocking(int fd, int should_block)
{
	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if (tcgetattr(fd, &tty) != 0) {
		printf("error %s from tggetattr", strerror(errno));
		return;
	}

	tty.c_cc[VMIN] = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 5;	// 0.5 seconds read timeout

	if (tcsetattr(fd, TCSANOW, &tty) != 0)
		printf("error %s setting term attributes", strerror(errno));
}


int avr_open()
{
	int fd = open(AVR_PORT, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %s opening %s: %s", strerror(errno), AVR_PORT,
		       strerror(errno));
		exit(EXIT_FAILURE);
	}

	set_interface_attribs(fd, B38400, 0);	// set speed to 38,400 bps, 8n1 (no parity)
	set_blocking(fd, 0);	// set no blocking

	return fd;
}

int main()
{
	printf("Starting AVR testing program\n");
	int avr_fd = avr_open();
	
	if(avr_fd <= 0) {
		perror("Unable to connect to the AVR");
		exit(EXIT_FAILURE);
	}

	uint8_t speed = 0;
	uint16_t RPM = 0;
	struct AVRPacket packet = {0x0000, 0x00, 0x0000};
	struct AVRPacket *data = &packet;
	
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
		packet.speed = speed;
		packet.rpm = ((RPM << 8) | (RPM >> 8));
		printf("Sent: %d bytes.\n", (int) write(avr_fd, data, sizeof(packet)));
		usleep(100000);
	}
	return 0;
}
