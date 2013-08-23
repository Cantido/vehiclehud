#include "config.h"
#if NORITAKE_INTERFACE==1 && NORITAKE_SERIAL==1

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "interface.h"

static struct {
    unsigned chip:5;
    enum { NONE, READ, WRITE } dir:2;
    unsigned rs:1;
} state;

void initPort() {
    RAISE(SIO);
    RAISE(CS);
    DIRECTION(CS, 1);
    DIRECTION(SIO, 1);
    DIRECTION(SCK, 1);
    state.dir = state.NONE;
    state.chip = state.rs = 0;
}

void hardReset() {
    initPort();
    LOWER(RESET);
    _delay_ms(100);
    RAISE(RESET);
    _delay_ms(100);
}

static void selectChip(bool chip) {
    if (state.chip != (chip? 16: 8)) {
        state.chip = chip? 16: 8;
        state.dir = state.NONE;
    }
}

static void _write(uint8_t data) {
    for (uint8_t i=0x80; i; i>>=1) {
        SETPIN(SIO, data & i);
        LOWER(SCK);
        RAISE(SCK);
    }
}

void writePort(uint8_t data, bool rs, bool chip) {
    selectChip(chip);
    if (state.dir != state.WRITE || state.rs != rs) {        
        RAISE(CS);
        _delay_us(.5);
        LOWER(CS);
        _write(0xe0 + state.chip + (1<<rs));
        state.dir = state.WRITE;
        state.rs = rs;
    }
    _write(data);
}

uint8_t readPort(bool rs, bool chip) {
    selectChip(chip);
    if (state.dir != state.READ || state.rs != rs) {
        RAISE(CS);
        _delay_us(.5);
        LOWER(CS);
        _write(0xe4 + state.chip + (1<<rs));
        state.dir = state.READ;
        state.rs = rs;
        _delay_us(1);
    }
    DIRECTION(SIO, 0);
    uint8_t data = 0;
    for (uint8_t i=0x80; i; i>>=1) {
        LOWER(SCK);
        RAISE(SCK);
        if (CHECK(SIO))
            data |= i;
    }
    DIRECTION(SIO, 1);
    return data;
}

#endif
