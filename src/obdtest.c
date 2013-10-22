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
	usleep(TIMEOUT);

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

int obd_open() {
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

int main()
{
    int fd = obd_open();

	int n = 0, x = 0;	//ints for keeping track of received/parsed bits
	long int A = 0, B = 0, C = 0, D = 0;	//translated bytes from the OBD chip
	int RPM = 0;		//calculated engine RPM
	char buf[50];		//buffer to store raw characters received from the OBD chip
	char buf2[50];		//buffer to store parsed characters
	char *pEnd;		//pointer for parsing OBD bytes

	memset(buf, 0, sizeof buf);	//clear the buffer

	printf("Resetting the chip...\n");
	write(fd, "ATZ\r", 4);

	//wait for the chip to reset
	usleep(1000000);

	printf("Finding a protocol...\n");
	write(fd, "0100\r", 5);
	usleep(2000000);

	//clear IO buffers
	tcflush(fd, TCIOFLUSH);

	//disable echos
	write(fd, "ATE0\r", 5);

	printf("Making sure chip is working...\n");
	do {
		obd_read(fd, buf, 10);
	} while (!strcmp(buf2, "ON"));

	//ready to go
	printf("OBD Ready\n");

	//set the read to blocking
	set_blocking(fd, 1);

	printf("Beginning RPM read cycle...\n");

	while (1) {
		printf("RPM read loop starting iteration...\n");
		//pause to keep from overloading the OBD chip
		usleep(1000000);

		//clear the buffers
		//memset (buf, 0, sizeof buf);
		//memset (buf2, 0, sizeof buf);

		//ask for RPM, tell the chip we only expect 1 line of data (should be faster)
		printf("Requesting RPM from OBD...\n");
		write(fd, "010C 1\r", 7);

		//read the response
		x = obd_read(fd, buf2, 13);

		printf("Recieved data: %s\n", buf2);

		if (!strcmp("STOPPED", buf2))
			printf("STOPPED\n");

		//if we got a proper RPM string, calculate and print RPM
		if (x == 12) {
			A = strtol(buf2, &pEnd, 16);
			B = strtol(pEnd, &pEnd, 16);
			C = strtol(pEnd, &pEnd, 16);
			D = strtol(pEnd, &pEnd, 16);

			RPM = (C * 256 + D) / 4;
			printf("RPM: %d\n", RPM);
		} else {
			printf("Didn't get a length 12 string, length is %d\n",
			       x);
		}
	}

	return 0;
}
