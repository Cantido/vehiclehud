#include "config.h"

#if NORITAKE_INTERFACE==0

#include "interface.h"

void initPort() {
    RAISE(RESET);
    DIRECTION(RS, 1);
    DIRECTION(RW, 1);
    DIRECTION(E, 1);
    DIRECTION(CS1, 1);
    DIRECTION(CS2, 1);
    DIRECTION(RESET, 1);
    *(&DATA_PORT - 1) = 0xff;
    readPort(true, true); // first read is 0
}

void hardReset() {
    LOWER(RESET);
    _delay_ms(100);
    RAISE(RESET);
    _delay_ms(100);
}

uint8_t readPort(bool rs, bool chip) {
    SETPIN(CS1, !chip);
    SETPIN(CS2, chip);
    SETPIN(RS, rs);
    *(&DATA_PORT - 1) = 0x00; // DDRx
    RAISE(RW);
    LOWER(E);
    RAISE(E);
    LOWER(E);
    RAISE(E);
    _delay_us(.32);
    uint8_t data = *(&DATA_PORT - 2); // PINx
    *(&DATA_PORT - 1) = 0xff; // DDRx
    return data;
}


void writePort(uint8_t data, bool rs, bool chip) {
    SETPIN(CS1, !chip);
    SETPIN(CS2, chip);
    SETPIN(RS, rs);
    LOWER(RW);
    RAISE(E);
    DATA_PORT = data;
    LOWER(E);
    LOWER(E);
}
#endif
