#ifndef AVR_H
#define AVR_H

struct AVRPacket {
	uint16_t header;	//byte 0, byte 1
	uint8_t speed;		//byte 2
	uint16_t rpm;		//byte 3, byte 4
	uint8_t tPos;		//byte 5
	uint8_t iTemp;		//byte 6
	uint8_t eLoad;		//byte 7
	uint8_t eTemp;		//byte 8
	uint16_t maf;		//byte 9, byte 10
	uint8_t tAdv;   //byte 11
} __attribute((packed)) __;


int avr_open(char* portname);
void send_to_avr(int avr_fd, int speed, int RPM);

#endif