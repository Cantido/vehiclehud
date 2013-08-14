#include "config.h"
#if NORITAKE_INTERFACE==1 && NORITAKE_SERIAL==3

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "interface.h"

void initPort() {
    RAISE(SCK);
    RAISE(RESET);
    DIRECTION(RS, 1);
    DIRECTION(SI, 1);
    DIRECTION(SCK, 1);
    DIRECTION(RESET, 1);
    DIRECTION(CS1, 1);
    DIRECTION(CS2, 1);
}

void hardReset() {
    initPort();
    LOWER(RESET);
    _delay_ms(100);
    RAISE(RESET);
    _delay_ms(100);
}

static void _write(uint8_t data) {
    for (uint8_t i=0x80; i; i>>=1) {
        SETPIN(SI, data & i);
        LOWER(SCK);
        RAISE(SCK);
    }
}

void writePort(uint8_t data, bool rs, bool chip) {
    LOWER(CS1);
    LOWER(CS2);
    SETPIN(CS1, !chip);
    SETPIN(CS2, chip);
    SETPIN(RS, rs);
    _write(data);
}

uint8_t readPort(bool rs, bool chip) {
    return 0;
}

#endif
