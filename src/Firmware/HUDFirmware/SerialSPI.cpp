#include "config.h"
#if NORITAKE_INTERFACE==1 && NORITAKE_SERIAL==2

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "interface.h"

void initPort() {
    RAISE(SCK);
    RAISE(RESET);
    RAISE(CS1);
    RAISE(CS2);
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
    RAISE(CS1);
    RAISE(CS2);
    SETPIN(CS1, chip);
    SETPIN(CS2, !chip);
    _write(0xf8 + (rs<<1));
    _write(data);
}

uint8_t readPort(bool rs, bool chip) {
    RAISE(CS1);
    RAISE(CS2);
    SETPIN(CS1, chip);
    SETPIN(CS2, !chip);
    _write(0xfc + (rs<<1));
    uint8_t data = 0;
    for (uint8_t i=0x80; i; i>>=1) {
        LOWER(SCK);
        RAISE(SCK);
        if (CHECK(SO))
            data |= i;
    }
    return data;
}

#endif
