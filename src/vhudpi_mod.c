#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

/*
 *
 *
 *	need dis
 *
 *
 */
#include <stdint.h>

//#define OBD_PORT "/dev/ttyUSB0"
/*
*
*
*	for BT adapter
*
*
*/
#define OBD_PORT "/dev/rfcomm0"

#define AVR_PORT "/dev/ttyUSB1"
#define TIMEOUT 500000

#define DISABLE 0
#define ENABLE 1

struct AVRPacket
{
	uint16_t 	header;	//byte 0, byte 1
	uint8_t		speed;	//byte 2
	uint16_t 	rpm;	//byte 3, byte 4
	uint8_t		tPos;	//byte 5
	uint8_t		iTemp;	//byte 6
	uint8_t		eLoad;	//byte 7
	uint8_t		eTemp;	//byte 8
	uint16_t 	maf;	//byte 9, byte 10
	uint8_t		tAdv;	//byte 11
};

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
	/*
	 *
	 *
	 *	added '?' cause that shows up sometimes
	 *	and it's annoying
	 *
	 */
	for (int i = 0; i < charsread; ++i) {
		if ((readbuf[i] != '>') && (readbuf[i] != '\r') && (readbuf[i] != '?')) {
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
	int fd = open(OBD_PORT, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %s opening %s: %s", strerror(errno), OBD_PORT,
		       strerror(errno));
		exit(EXIT_FAILURE);
	}

	/*
	 *
	 *
	 *	baud rate should be defined above
	 *
	 *
	 */
	set_interface_attribs(fd, B38400, 0);	// set speed to 38,400 bps, 8n1 (no parity)
	set_blocking(fd, 0);	// set no blocking

	return fd;
}

int avr_open()
{
	int fd = open(AVR_PORT, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %s opening %s: %s", strerror(errno), AVR_PORT,
		       strerror(errno));
		exit(EXIT_FAILURE);
	}

	/*
	 *
	 *
	 *	baud rate should be defined above
	 *
	 *
	 */
	set_interface_attribs(fd, B38400, 0);	// set speed to 38,400 bps, 8n1 (no parity)
	set_blocking(fd, 0);	// set no blocking

	return fd;
}

void obd_wait_until_on(int fd)
{
	int max_iter = 100;
	char buf[50];
	

	for (int i = 0; i < max_iter; i++) 
	{
		/*
		 *
		 *
		 *	pretty sure you need to keep sending ATIGN
		 *	maybe
		 *
		 */
		write(fd, "ATIGN\r", 6);
		obd_read(fd, buf, 10);
		if (strstr(buf, "ON") != NULL) {
			return;
		}
		usleep(10000);
	}
	printf("Device was not ready in an acceptable amount of time\n");
	exit(EXIT_FAILURE);
}

/*
 *
 *
 *	Keep asking the ELM to disable echos
 *
 *
 */
void obd_wait_until_echo(int fd, bool enable)
{
	int max_iter = 100;
	char buf[50];
	

	for (int i = 0; i < max_iter; i++) 
	{
		/*
		 *
		 *
		 *	ternary cause they're cool
		 *
		 *
		 */
		write(fd, (enable ? "ATE1\r" : "ATE0\r"), 5);
		obd_read(fd, buf, 10);
		if (strstr(buf, "OK") != NULL) {
			return;
		}
		usleep(10000);
	}
	printf("Echo was not set in an acceptable amount of time\n");
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
	short int measureme = 0;
	char buf[50];
	char *pEnd;
	long int *byteptr;
	
	/* With echoes disabled, he OBD data responses look like this:
	 *
	 * 41 0c XX XX\r\n
	 * >
	 *
	 * if we requested RPM. That adds up to 17 characters after the numbytes * 2 hex chars
	 */
	
	//size_t numchars = 6 + (numbytes * 3 - 1) + 2; // ack + data w/ spaces + newlines


	/*
	 *
	 *
	 *	maybe +1 for '>' character
	 *
	 *
	 */
	size_t numchars = (numbytes * 3 - 1) + 1; //data w/ spaces + newline 

	charsread = obd_read(fd, buf, numchars);
	if (charsread == numchars) {
		/*
		 *
		 *
		 *	added cast
		 *	sizeof was not working, so I just did times 8
		 *	seems to work
		 *
		 */
		byteptr = (long int*)malloc(8*numbytes);

		byteptr[0] = strtol(buf, &pEnd, 16);
		for (short int i = 1; i < numbytes; i++) {
			byteptr[i] = strtol(pEnd, &pEnd, 16);
		}
	/*
	 *
	 *
	 *	STOPPED will most likely happen at some point
	 *	probably best not to quit when we receive it
	 *
	 */
	//} else if(strstr(buf, "STOPPED") != NULL) {
	//	fprintf(stderr, "OBD responded \"STOPPED\"\n");
	} else {
		byteptr = NULL;
	}
	return byteptr;
}

int get_rpm(int fd)
{
	long int *data;
	long int A, B;
	int rpm;

	write(fd, "010C 1\r", 7);
	/*
	 *	RPM reponse should be 41 0C AB CD
	 *	that's four bytes
	 *	
	 *
	 *
	 */
	data = obd_get_bytes(fd, 4);

	if (data == NULL) {
		rpm = -1;
	} else {
		A = data[2];
		B = data[3];
		rpm = (A * 256 + B) / 4;
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
		speed = (int)data[0];
		free(data);
	}
	return speed;
}

int main()
{
	int fd = obd_open();

	/*
	 *
	 *
	 *	AVR stuff
	 *
	 *
	 */
	int avr_fd = avr_open();
	uint8_t data[5] = {0x37, 0x00, 0x60, 0x03, 0x09};
	
	//AVRPacket packet = {0x0000, 0x00, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x0000, 0x00};
	//packet* data = packet;
	
	uint16_t avr_RPM;

	int RPM = 0;		//calculated engine RPM
	int speed = 0;		//vehicle speed
	char buf[50];

	/*
	 *
	 *
	 *	temp stuff for wait until '>' char shows up
	 *
	 *
	 */
	char temp_buf[2];
	int temp_read = 0;

	memset(buf, 0, sizeof buf);	//clear the buffer

	printf("Resetting the chip...\n");
	obd_reset(fd);

	printf("Finding a protocol...\n");
	obd_find_protocol(fd);

	//clear IO buffers
	tcflush(fd, TCIOFLUSH);

	/*
	 *
	 *
	 *	keep trying to turn OFF echos until the ELM complies
	 *
	 *
	 */
	obd_wait_until_echo(fd, false);

	printf("Making sure chip is working...\n");
	obd_wait_until_on(fd);

	//ready to go
	printf("OBD Ready\n");

	//set the read to blocking
	set_blocking(fd, 1);

	printf("Beginning read cycle...\n");

	while (1) 
	{
		/*
		 *
		 *
		 *	need to wait
		 *
		 *
		 */
		usleep(60000);
		RPM = get_rpm(fd);

		//usleep(50000);
		//speed = get_speed(fd);

		/*
		 *
		 *
		 *	If we got legitimate numbers, print them
		 *	also send them to the AVR using uints
		 *
		 */
		if ((RPM != -1) && (speed != -1))
		{
			avr_RPM = RPM;
			data[2] = speed;
			data[3] = (avr_RPM >> 8);
			data[4] = avr_RPM;
			
			//packet.header = 0x00;
			//packet.speed = speed;
			//packet.rpm = avr_RPM;
			//write(avr_fd, data, sizeof(packet));
			write(avr_fd, data, 5);
			printf("RPM: %d\nSpeed: %d\n", RPM, speed);
		}

		/*
		 *
		 *
		 *	flushing IO buffers gets rid of random responses
		 *
		 *
		 */
		tcflush(fd, TCIOFLUSH);

		/*
		 *
		 *
		 *	wait to get '>' character before sending new request
		 *	doesn't work :(
		 *
		 */
/*
		do
		{
			temp_read = read(fd, temp_buf, 1);
			usleep(1000);
		} while ((temp_read != 1) && (temp_buf[0] != '>')); */
	}

	return 0;
}
