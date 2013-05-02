#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>

#define PORTNAME "/dev/ttyS0"
#define BAUDRATE       B9600
#define DATABITS           1
#define NEWLINE          '\r'
#define READTIMEOUT     5000
#define WRITETIMEOUT    5000


int main(int argc, char** argv) {
    struct termios obdcfg;
    int obd_fd;
   
    /*
     * Configuring serial communication with the ELM327:
     *
     *  Baud rate: 9600
     *  Data Bits: 8 bits
     *  Parity: No parity
     *  Stop Bits: 1
     *  Newline Character: \r
     */
    
    if((obd_fd = open(PORTNAME, obd_fd, O_RDWR | O_NOCTTY | O_NDELAY )) < 0 || !isatty(obd_fd)) {
        perror("Error opening serial file IO");
        exit(EXIT_FAILURE);
    }
    
    obdcfg.c_iflag |= ( ICRNL );
    obdcfg.c_oflag |= ( ONLRET );
    obdcfg.c_cflag |= ( CS8 );
    
    if(cfsetispeed(&obdcfg, BAUDRATE) < 0 || cfsetospeed(&obdcfg, BAUDRATE) < 0) {
        perror("Error setting baud rate");
        exit(EXIT_FAILURE);
    }
    
    if(tcsetattr(obd_fd, TCSANOW, &obdcfg) < 0) {
        perror("Error setting serial attributes");
        exit(EXIT_FAILURE);
    }
    
    printf("Serial connection established\n");
    
    return 0;
}
