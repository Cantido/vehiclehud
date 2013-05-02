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

/*
     * Configuring serial communication with the ELM327:
     *
     *  Baud rate: 9600
     *  Data Bits: 8 bits
     *  Parity: No parity
     *  Stop Bits: 1
     *  Newline Character: \r
     */

#define TTYPREFIX "/dev/tty"
#define BAUDRATE            B9600
#define DATABITS                1
#define NEWLINE               '\r'
#define READTIMEOUT          5000
#define WRITETIMEOUT         5000

int serial_connect();


int main(int argc, char** argv) {
    int obd_fd = 0;
    
    obd_fd = serial_connect();
    
    return 0;
}

int serial_connect() {
    struct termios obdcfg;
    int obd_fd = 0;
    char portname[11];
    int conn_success_flag = 0;
    
    printf("Attempting to connect to a serial port...\n");
    
    for(int i = 0; (i < 10) && (conn_success_flag != 1); i++) {
        sprintf(portname, "%sS%d", TTYPREFIX, i);
        printf("...Trying %s...", portname);
        fflush(stdout);
        if((obd_fd = open(portname, obd_fd, O_RDWR | O_NOCTTY )) < 0 || !isatty(obd_fd)) {
            perror("Failure");
        } else {
            printf("Success!\n");
            conn_success_flag = 1;
        }
    }
    
    if(conn_success_flag != 1) {
        fprintf(stderr, "Cannot connect to a serial port. Exiting.\n");
        exit(EXIT_FAILURE);
    }
    
    obdcfg.c_iflag |= ( ICRNL );  // Translate CR to NL on input
    //obdcfg.c_oflag |= ();
    obdcfg.c_cflag |= ( CS8 );    // Character size: 8 bits
    
    if(cfsetispeed(&obdcfg, BAUDRATE) < 0 || cfsetospeed(&obdcfg, BAUDRATE) < 0) {
        perror("Error setting baud rate");
        exit(EXIT_FAILURE);
    }
    
    printf("Setting serial attributes...");
    fflush(stdout);
    
    if(tcsetattr(obd_fd, TCSANOW, &obdcfg) < 0) {
        perror("Failure");
        exit(EXIT_FAILURE);
    } else {
        printf("Success!\n");
    }
    
    printf("Serial connection established\n");
    
    return obd_fd;
}
