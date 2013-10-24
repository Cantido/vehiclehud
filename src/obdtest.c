#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

bool obd_reset(int fd)
{
	char buf[50];
	write(fd, "ATZ\r", 4);
	usleep(1000000);
	if (obd_read(fd, buf, 11) >= 10)
		return true;
	else
		return false;
}

bool obd_find_protocol(int fd)
{
	char buf[50];
	write(fd, "0100\r", 5);
	usleep(2000000);
	if (obd_read(fd, buf, 18) >= 17)
		return true;
	else
		return false;
}

void obd_enable_echo(int fd, bool enable)
{
	if (enable)
		write(fd, "ATE1\r", 5);
	else
		write(fd, "ATE0\r", 5);
}

long int *obd_get_bytes(int fd, size_t numbytes)
{
	int charsread;
	char buf[50];
	char *pEnd;
	long int *byteptr;

	size_t numspaces = numbytes - 1;
	size_t numnewlines = 2;
	size_t numchars = (numbytes * 2) + numspaces + numnewlines;

	charsread = obd_read(fd, buf, numchars);

	if (charsread == numchars) {
		byteptr = malloc(sizeof(short int) * numbytes);

		byteptr[0] = strtol(buf, &pEnd, 16);
		for (short int i = 1; i < numbytes; i++) {
			byteptr[i] = strtol(pEnd, &pEnd, 16);
		}
	} else {
		byteptr = NULL;
	}
	return byteptr;
}

int get_rpm(int fd)
{
	long int *data;
	long int C, D;
	int rpm;

	write(fd, "010C 1\r", 7);
	data = obd_get_bytes(fd, 4);

	if (data == NULL) {
		rpm = -1;
	} else {
		C = data[2];
		D = data[3];
		rpm = (C * 256 + D) / 4;
	}
	free(data);
	return rpm;
}

int get_speed(int fd)
{
	long int *data;
	int speed = 0;

	write(fd, "010D 1\r", 7);
	data = obd_get_bytes(fd, 3);

	if (data == NULL) {
		speed = -1;
	} else {
		speed = (int)data[2];
	}
	free(data);
	return speed;
}

int main()
{
	int fd = obd_open();

	int RPM = 0;		//calculated engine RPM
	int speed = 0;		//vehicle speed
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

	printf("Beginning read cycle...\n");

	while (1) {
		RPM = get_rpm(fd);
		speed = get_speed(fd);

		printf("RPM: %d\nSpeed: %d\n", RPM, speed);
	}

	return 0;
}
