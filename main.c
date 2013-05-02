/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 *  Authors: Robert Richter and Aaron Hall                                 *
 *                                                                         *
 *  Copyright 2013                                                         *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
        fprintf(stderr, "[%s]: Error opening serial port at %s: ", argv[0], PORTNAME);
        perror("");
        exit(EXIT_FAILURE);
    }
    
    obdcfg.c_iflag |= ( ICRNL );
    obdcfg.c_oflag |= ( ONLRET );
    obdcfg.c_cflag |= ( CS8 );
    
    if(cfsetispeed(&obdcfg, BAUDRATE) < 0 || cfsetospeed(&obdcfg, BAUDRATE) < 0) {
        fprintf(stderr, "[%s]: Error setting baud rate at %s: ", argv[0], PORTNAME);
        perror("");
        exit(EXIT_FAILURE);
    }
    
    if(tcsetattr(obd_fd, TCSANOW, &obdcfg) < 0) {
        fprintf(stderr, "[%s]: Error setting serial attributes for %s: ", argv[0], PORTNAME);
        perror("");
        perror("Error setting serial attributes");
        exit(EXIT_FAILURE);
    }
    
    printf("Serial connection established\n");
    
    return 0;
}
