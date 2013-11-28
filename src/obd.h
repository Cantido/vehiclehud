#ifndef OBD_H
#define OBD_H


#define TIMEOUT 500000
#define MAX_CHARS 50

#define DISABLE 0
#define ENABLE 1

int obd_open(char* portname);
void set_blocking(int fd, int should_block);

bool obd_reset(int obd_fd);
void obd_wait_until_on(int obd_fd);
void obd_wait_until_echo(int fd, bool enable);

bool obd_find_protocol(int fd);
int get_rpm(int fd);
int get_speed(int fd);
int get_throttle(int fd);
int get_intake_temp(int fd);
int get_engine_load(int fd);
int get_engine_temp(int fd);
int get_maf(int fd);
int get_timing_adv(int fd);

#endif