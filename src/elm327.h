#ifndef ELM327_H
#define ELM327_H

#define TTYPREFIX   "/dev/ttyUSB"
#define BAUDRATE        B9600
#define DATABITS            1
#define LINEEND           '\r'
#define READTIMEOUT      5000
#define WRITETIMEOUT     5000

int serial_connect();
void obd_init(const int obd_fd);

int get_elm327_data(int obd_fd, char* request);

#endif
