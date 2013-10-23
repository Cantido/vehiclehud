#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#define PORTNAME "/dev/ttyUSB0"
#define TIMEOUT 500000

#define DISABLE 0
#define ENABLE 1

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

int obd_read(int fd, char *buf, size_t count)
{
	int charsprocessed = 0;
	char readbuf[50];
	int charsread = read(fd, readbuf, count);

	//get rid of any worthless characters
	for (int i = 0; i < charsread; ++i) {
		if ((readbuf[i] != '>') && (readbuf[i] != '\r')) {
			buf[charsprocessed] = readbuf[i];
			++charsprocessed;
		}
	}
	//terminate the parsed string
	buf[charsprocessed + 1] = '\0';
	return charsprocessed;
}

void set_baud_115200(int fd)
{

	//set baud rate to 115200
	write(fd, "ATBRD 23\r", 8);
	usleep(200000);

	//change our baud rate and ack
	set_interface_attribs(fd, B115200, 0);
	write(fd, "\r", 1);

	usleep(1000000);
}

int obd_open()
{
	int fd = open(PORTNAME, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %s opening %s: %s", strerror(errno), PORTNAME,
		       strerror(errno));
		exit(EXIT_FAILURE);
	}

	set_interface_attribs(fd, B38400, 0);	// set speed to 38,400 bps, 8n1 (no parity)
	set_blocking(fd, 0);	// set no blocking

	return fd;
}

void obd_wait_until_on(int fd)
{
	int max_iter = 100;
	char buf[50];

	for (int i = 0; i < max_iter; i++) {
		obd_read(fd, buf, 10);
		if (strstr(buf, "ON") != NULL) {
			return;
		}
		usleep(1000);
	}
	printf("Device was not ready in an acceptable amount of time\n");
	exit(EXIT_FAILURE);
}

void obd_reset(int fd)
{
	write(fd, "ATZ\r", 4);

	//wait for the chip to reset
	usleep(1000000);
}

void obd_find_protocol(int fd)
{
	write(fd, "0100\r", 5);
	usleep(2000000);
}

void obd_enable_echo(int fd, int enable)
{
	if (enable == ENABLE) {
		write(fd, "ATE1\r", 5);
	} else {
		write(fd, "ATE0\r", 5);
	}
}

int get_rpm(int fd)
{
	int charsread;
	char buf[50];
	char *pEnd;
	long int A = 0, B = 0, C = 0, D = 0;
	int rpm;

	write(fd, "010C 1\r", 7);
	charsread = obd_read(fd, buf, 13);

	//if we got a proper RPM string, calculate and print RPM
	if (charsread = 12) {
		A = strtol(buf, &pEnd, 16);
		B = strtol(pEnd, &pEnd, 16);
		C = strtol(pEnd, &pEnd, 16);
		D = strtol(pEnd, &pEnd, 16);

		rpm = (C * 256 + D) / 4;
	} else {
		rpm = -1;
	}
	return rpm;
}

int main()
{
	int fd = obd_open();

	int RPM = 0;		//calculated engine RPM
	char buf[50];

	memset(buf, 0, sizeof buf);	//clear the buffer

	printf("Resetting the chip...\n");
	obd_reset(fd);

	printf("Finding a protocol...\n");
	obd_find_protocol(fd);

	//clear IO buffers
	tcflush(fd, TCIOFLUSH);

	obd_enable_echo(fd, DISABLE);

	printf("Making sure chip is working...\n");
	obd_wait_until_on(fd);

	//ready to go
	printf("OBD Ready\n");

	//set the read to blocking
	set_blocking(fd, 1);

	printf("Beginning RPM read cycle...\n");

	while (1) {
		printf("RPM read loop starting iteration...\n");
		usleep(1000000);

		printf("Requesting RPM from OBD...\n");
		RPM = get_rpm(fd);

		printf("RPM: %d\n", RPM);
	}

	return 0;
}
