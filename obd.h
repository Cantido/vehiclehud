#ifndef OBD_H
#define OBD_H

#define TTYPREFIX   "/dev/tty"
#define BAUDRATE        B9600
#define DATABITS            1
#define LINEEND           '\r'
#define READTIMEOUT      5000
#define WRITETIMEOUT     5000

#define PID_RPM "010C"

int serial_connect();
void obd_init(const int obd_fd);

int get_data(int obd_fd, char* request);

int get_RPM(int obd_fd);

#endif
