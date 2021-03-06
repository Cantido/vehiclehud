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

int obd_open(char* portname)
{
	int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %s opening %s: %s", strerror(errno), portname, strerror(errno));
		exit(EXIT_FAILURE);
	}

	set_interface_attribs(fd, B38400, 0);	// set speed to 38,400 bps, 8n1 (no parity)
	set_blocking(fd, 0);	// set no blocking

	return fd;
}

int obd_read(int fd, char *buf)
{
	int valid_chars = 0;
	char current_char;

	while ((read(fd, &buf[valid_chars], 1) == 1) && (valid_chars < 50)) {
		current_char = buf[valid_chars];

		if (current_char == '\r' || current_char == '\n') {
			continue;	// skip this character
		} else if (current_char == '>') {
			if (current_char == 0)
				continue;	// eat this carat if it's the first character
			else
				break;
		} else {
			valid_chars++;
		}
	}

	buf[valid_chars] = '\0';

	tcflush(fd, TCIOFLUSH);

	return valid_chars;
}

bool obd_reset(int fd)
{
	char buf[50];
	write(fd, "ATZ\r", 4);

	usleep(1000000);

	obd_read(fd, buf);

	if (strcmp(buf, "ELM327 v1.5") == 0)
		return true;
	else {
		return false;
	}
}

void obd_wait_until_on(int fd)
{
	int max_iter = 100;
	char buf[50];

	for (int i = 0; i < max_iter; i++) {
		write(fd, "ATIGN\r", 6);
		obd_read(fd, buf);
		if (strstr(buf, "ON") != NULL) {
			return;
		}
		usleep(10000);
	}
	printf("Device was not ready in an acceptable amount of time\n");
	exit(EXIT_FAILURE);
}

void obd_wait_until_echo(int fd, bool enable)
{
	int max_iter = 10;
	char buf[50];

	for (int i = 0; i < max_iter; i++) {
		write(fd, (enable ? "ATE1\r" : "ATE0\r"), 5);
		obd_read(fd, buf);
		if (strstr(buf, "OK") != NULL) {
			return;
		}
		usleep(10000);
	}
	printf("Echo was not set in an acceptable amount of time\n");
	exit(EXIT_FAILURE);
}

bool obd_find_protocol(int fd)
{
	char buf[50];
	write(fd, "0100\r", 5);
	usleep(2000000);
	if (obd_read(fd, buf) >= 17)
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

	size_t expected = 6 + (numbytes * 3 - 1) + 1;	// ack + data w/ spaces + newline 

	charsread = obd_read(fd, buf);

	if (charsread == expected) {
		byteptr = (long int *)malloc(8 * (numbytes + 2));

		byteptr[0] = strtol(buf, &pEnd, 16);
		for (short int i = 1; i < numbytes + 2; i++) {
			byteptr[i] = strtol(pEnd, &pEnd, 16);
		}
	} else if (strstr(buf, "STOPPED") != NULL) {
		fprintf(stderr, "OBD responded \"STOPPED\"\n");
		byteptr = NULL;
	} else if (strstr(buf, "UNABLE TO CONNECT") != NULL) {
		fprintf(stderr,
			"OBD responded \"UNABLE TO CONNECT\", exiting.\n");
		exit(EXIT_FAILURE);
	} else {
		byteptr = NULL;
	}
	return byteptr;
}

int get_rpm(int fd)
{
	long int *data;
	int rpm;

	write(fd, "010C 1\r", 7);
	data = obd_get_bytes(fd, 2);

	if (data == NULL) {
		rpm = -1;
	} else {

		rpm = ((data[2] * 256) + data[3]) / 4;
		free(data);
	}

	return rpm;
}

int get_speed(int fd)
{
	long int *data;
	int speed = 0;

	write(fd, "010D 1\r", 7);
	data = obd_get_bytes(fd, 1);

	if (data == NULL) {
		speed = -1;
	} else {
		speed = (int)data[2];
		free(data);
	}
	return speed;
}

int get_throttle(int fd)
{
	long int *data;
	int throttle = 0;

	write(fd, "0111 1\r", 7);

	data = obd_get_bytes(fd, 1);

	if (data == NULL) {
		throttle = -1;
	} else {
		throttle = (int)data[2] * 100 / 255;
		free(data);
	}
	return throttle;
}

int get_intake_temp(int fd)
{
	long int *data;
	int intake_temp = 0;

	write(fd, "010F 1\r", 7);

	data = obd_get_bytes(fd, 1);

	if (data == NULL) {
		intake_temp = -1;
	} else {
		intake_temp = (int)data[2] - 40;
		free(data);
	}
	return intake_temp;
}

int get_engine_load(int fd)
{
	long int *data;
	int engine_load = 0;

	write(fd, "0104 1\r", 7);

	data = obd_get_bytes(fd, 1);

	if (data == NULL) {
		engine_load = -1;
	} else {
		engine_load = (int)data[2] * 100 / 255;
		free(data);
	}
	return engine_load;
}

int get_engine_temp(int fd)
{
	long int *data;
	int engine_temp = 0;

	write(fd, "0105 1\r", 7);

	data = obd_get_bytes(fd, 1);

	if (data == NULL) {
		engine_temp = -1;
	} else {
		engine_temp = (int)data[2] - 40;
		free(data);
	}
	return engine_temp;
}

int get_maf(int fd)
{
	long int *data;
	int maf = 0;

	write(fd, "0110 1\r", 7);

	data = obd_get_bytes(fd, 1);

	if (data == NULL) {
		maf = -1;
	} else {
		maf = (int)data[2] / 100;
		free(data);
	}
	return maf;
}

int get_timing_adv(int fd)
{
	long int *data;
	int timing_adv = 0;

	write(fd, "010E 1\r", 7);

	data = obd_get_bytes(fd, 1);

	if (data == NULL) {
		timing_adv = -1;
	} else {
		timing_adv = (int)data[2] / 2 - 64;
		free(data);
	}
	return timing_adv;
}