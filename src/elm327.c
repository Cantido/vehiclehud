#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <config.h>

#include "elm327.h"

/* serial_connect: Attempts to open up & configure a file descriptor
 *  for a serial connection to the ELM327.
 * 
 *  If there is any sort of failure, the program exits.
 *
 * Preconditions: None
 * Postconditions: A serial port /dev/ttySX is reconfigured
 * Returns: A file descriptor to the serial port
 *
 */

int serial_connect() {
    struct termios obdcfg;
    int obd_fd = 0;
    char portname[11];
    int conn_success_flag = 0;
    
    /*
     * Step 1: Open a file descriptor to /dev/ttySX
     *
     *  Method: Iterate through /dev/ttyS0 through /dev/ttyS9,
     *    and see if any work.
     */
    
    printf("Attempting to connect to a serial port...\n");
    
    for(int i = 0; (i < 10) && (conn_success_flag != 1); i++) {
        sprintf(portname, "%sS%d", TTYPREFIX, i);
        printf("...Trying %s...", portname);
        fflush(stdout);
        if((obd_fd = open(portname, obd_fd, O_RDWR | O_NOCTTY | O_NONBLOCK )) < 0 || !isatty(obd_fd)) {
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
    
    /*
     * Step 2: Set termios flags
     *
     *  Input flags:   ICRNL: Translate CR to NL on input
     *  Output flags:  none
     *  Control flags: CS8: Set character size to 8 bits
     *
     *  Input and Output baud rate set to BAUDRATE
     */
    
    obdcfg.c_iflag |= ( ICRNL );
    //obdcfg.c_oflag |= ();
    obdcfg.c_cflag |= ( CS8 );
    
    if(cfsetispeed(&obdcfg, BAUDRATE) < 0 || cfsetospeed(&obdcfg, BAUDRATE) < 0) {
        perror("Error setting baud rate");
        exit(EXIT_FAILURE);
    }
    
    /*
     * Step 3: Apply termios attributes to the file descriptor
     */
    
    printf("Setting serial attributes...");
    fflush(stdout);
    
    if(tcsetattr(obd_fd, TCSANOW, &obdcfg) < 0) {
        perror("Failure");
        exit(EXIT_FAILURE);
    } else {
        printf("Success!\n");
    }
    
    /*
     * Done!
     */
     
    printf("Serial connection established\n");
    
    return (obd_fd);
}

/* obd_init: Applies settings to the ELM327 
 *  and makes sure that it is ready to communicate.
 *
 * Arguments: a file descriptor to a connection with the ELM
 * Preconditions: a connection with the ELM has been established
 * Postconditions: the ELM is configured and is ready to use
 * Returns: Nothing
 *
 */

void obd_init(const int obd_fd) {
     /* ---------------------------------------------------------------------
     * Initializing the ELM327:
     *  1. Reset the chip by sending it the string "ATZ"
     *  2. (optional) verify that the chip has sent "ELM327 v1.4b"
     *  3. Disable command echoing from the chip with "ATE0"
     *  4. Ask the chip to find a protocol with "ATSP0"
     *
     * Note: All strings must be terminated by a carriage return.
     * ---------------------------------------------------------------------*/
    ssize_t charsread;
    char buf[BUFSIZ];
    
    write(obd_fd, "ATZ\r", 4);
    
    charsread = read(obd_fd, buf, sizeof(buf));
    buf[charsread+1] = '\0';
    
    printf("Read a total of %d characters:\n%s", (int) charsread, buf);
    
    return;
}

int get_elm327_data(int obd_fd, char* request) {
    static int response;
    char buf[BUFSIZ];
    
    printf("Sending string \"%s\" to ELM327\n", request);
    
    // replacing final character of request string with carriage return
    request[sizeof(request)] = LINEEND;
    
    write(obd_fd, request, sizeof(request));
    usleep(100);
    read(obd_fd, buf, sizeof(buf));
    
    // TODO: error check first two bytes
    
    printf("Received \"%s\" from ELM327\n", buf);
    
    response = (int) strtol(buf+2, NULL, 16);
    
    return (response);
}
