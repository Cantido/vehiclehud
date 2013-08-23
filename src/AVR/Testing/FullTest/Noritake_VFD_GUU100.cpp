#include "config.h"
#include "Noritake_VFD_GUU100.h"
#include "interface.h"
#include <avr/pgmspace.h>
#include <stdlib.h>

#define CHIP 64

static uint8_t bitsBetween(uint8_t a, uint8_t b) {
    #if NORITAKE_VERTICAL == 1
        if (a / 8 != b / 8)
            return (1 << (8 - a % 8)) - 1;
        a = 8 - a % 8;
        b = 8 - b % 8;
        return ((1 << a) - 1) & ~((1 << b) - 1);
    #else
        if (a / 8 != b / 8)
            return ~((1 << a % 8) - 1);
        a = a % 8;
        b = b % 8;
        return ~((1 << a) - 1) & ((1 << b) - 1);
    #endif

}

void Noritake_VFD_GUU100::displayOn() {
    command(0x3f, 0);
    command(0x3f, 1);
    setScreenBrightness(brightness);
    display_on = true;
}

void Noritake_VFD_GUU100::displayOff(bool powerSave) {
    power_save = powerSave;
    display_on = false;
    command(0x3e, 0);
    command(0x3e, 1);
    if (powerSave)
        command(0x20, 0);
}

void Noritake_VFD_GUU100::setScreenBrightness(uint8_t percent) {
    if (percent <= 100) {
        power_save = false;
        brightness = 8 - (percent+12) * 10 / 125;
        command(0x28 + brightness, 0);
        command(0x28 + brightness, 1);
    }
}

void Noritake_VFD_GUU100::reset() {
    initPort();
    hardReset();
}

void Noritake_VFD_GUU100::init() {
    initPort();
    setFont(0, 0, 0);
    fontGutter = 1;
    setScreenBrightness(100);
    command(0xc0, 0); // display start line 0
    command(0xc0, 1); // display start line 0
    displayOn();
    #if NORITAKE_VERTICAL == 1
        command(0x40, 0); // y = 0
        command(0x40, 1); // y = 0
        command(0xb8 + (NORITAKE_WIDTH - 1) / 8 % 8, 0); // x = 0
        command(0xb8 + (NORITAKE_WIDTH - 1) / 8 % 8, 1); // x = 0
    #else
        command(0x40, 0); // x = 0
        command(0x40, 1); // x = 0
        command(0xb8, 0); // y = 0
        command(0xb8, 1); // y = 0
    #endif
    cur_x = 0;
    cur_y = 0;
}

void Noritake_VFD_GUU100::increment() {
    #if NORITAKE_VERTICAL == 1
        cur_y++;
        if (cur_y % CHIP == 0) {
            bool chip = cur_y / CHIP;
            if (cur_y == 0)
                cur_x = (cur_x >= NORITAKE_WIDTH - 8)? 0: cur_x + 8;
            command(0x40, chip);
            command(0xb8 + (NORITAKE_WIDTH - 1 - cur_x) / 8 % 8, chip);
        }
    #else
        cur_x++;
        if (cur_x % CHIP == 0) {
            bool chip = cur_x / CHIP;
            if (cur_x == 0)
                cur_y = (cur_y >= NORITAKE_HEIGHT - 8)? 0: cur_y + 8;
            command(0x40, chip);
            command(0xb8 + cur_y / 8 % 8, chip);
        }
    #endif
}

void Noritake_VFD_GUU100::writeData(uint8_t data) {
    #if NORITAKE_VERTICAL == 1
        writePort(data, true, cur_y / CHIP);
    #else
        writePort(data, true, cur_x / CHIP);
    #endif
    increment();
}

uint8_t Noritake_VFD_GUU100::readData() {
    #if NORITAKE_VERTICAL == 1
        uint8_t data = readPort(true, cur_y / CHIP);
    #else
        uint8_t data = readPort(true, cur_x / CHIP);
    #endif
    increment();
    return data;
}

void Noritake_VFD_GUU100::command(uint8_t bits, bool chip) {
    writePort(bits, false, chip);
}

uint8_t Noritake_VFD_GUU100::readStatus(bool chip) {
    return readPort(false, chip);
}

void Noritake_VFD_GUU100::invertOn() {
    invert = true;
}
void Noritake_VFD_GUU100::invertOff() {
    invert = false;
}

void Noritake_VFD_GUU100::setCursor(uint8_t x, uint8_t y) {
    if (x >= NORITAKE_WIDTH || y >= NORITAKE_HEIGHT)
        return;
    #if NORITAKE_VERTICAL == 1
        if (y != cur_y)
            command(0x40 + y % CHIP, y / CHIP);
        if ((y / CHIP != cur_y / CHIP) || (x / 8 != cur_x / 8))
            command(0xb8 + (NORITAKE_WIDTH - 1 - x) / 8 % 8, y / CHIP);
    #else
        if (x != cur_x)
            command(0x40 + x % CHIP, x / CHIP);
        if (x / CHIP != cur_x / CHIP || y / 8 != cur_y / 8)
            command(0xb8 + y / 8 % 8, x / CHIP);
    #endif
    cur_y = y;
    cur_x = x;
}

void Noritake_VFD_GUU100::setDot(uint8_t x, uint8_t y, bool on) {
    if (x >= NORITAKE_WIDTH || y >= NORITAKE_HEIGHT)
        return;
    if (invert) on = !on;
    uint8_t data = peek(x, y);
    #if NORITAKE_VERTICAL == 1
        uint8_t change = 1 << (7 - x % 8);
    #else
        uint8_t change = 1 << (y % 8);
    #endif
    data = on? (data | change): (data & ~change);
    writeData(data);
}

void Noritake_VFD_GUU100::clearScreen() {
    home();
    for (unsigned i=0; i<NORITAKE_WIDTH*NORITAKE_HEIGHT/8; i++)
        writeData(0);
}
void Noritake_VFD_GUU100::drawImage(const uint8_t *data, uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    #if NORITAKE_VERTICAL == 1
        drawImage(data, x, y, width, height, height);
    #else
        drawImage(data, x, y, width, height, width);
    #endif
}

void Noritake_VFD_GUU100::drawImage(const uint8_t *data, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t stride) {
    if (x >= NORITAKE_WIDTH || y >= NORITAKE_HEIGHT)
        return;
    if (x + width >= NORITAKE_WIDTH)
        width = NORITAKE_WIDTH - x;
    if (y + height >= NORITAKE_HEIGHT)
        height = NORITAKE_HEIGHT - y;
    uint8_t inverter = invert? 0xff: 0x00;

    #if NORITAKE_VERTICAL == 1
        uint8_t x2 = x + width;
        if (clip(x) == clip(x2)) { // within one block
            uint8_t mask = bitsBetween(x, x2);
            for (uint8_t iy = 0; iy < height; iy++) {
                uint8_t mix = peek(clip(x), y + iy);
                uint8_t src = pgm_read_byte(data + iy);
                src >>= x % 8;
                src = (mix & ~mask) | (src & mask);
                writeData(src ^ inverter);
            }
        } else if (x == clip(x)) { // left is aligned
            for (uint8_t ix = x; ix < clip(x2); ix += 8) {
                setCursor(ix, y);
                for (uint8_t iy = 0; iy < height; iy++)
                    writeData(pgm_read_byte(data + iy) ^ inverter);
                data += stride;
            }
            if (x2 != clip(x2)) { // unaligned right
                uint8_t mask = bitsBetween(clip(x2), x2);
                for (uint8_t iy = 0; iy < height; iy++) {
                    uint8_t mix = peek(clip(x2), y + iy);
                    uint8_t src = pgm_read_byte(data + iy);
                    src = (mix & ~mask) | (src & mask);
                    writeData(src ^ inverter);
                }
            }
        } else { // left is unaligned
            uint8_t mask = bitsBetween(x, align(x));
            const uint8_t *end = data + stride * width;
            for (uint8_t iy = 0; iy < height; iy++) {
                uint8_t mix = peek(clip(x), y + iy);
                uint8_t src = pgm_read_byte(data + iy);
                src >>= x % 8;
                src = (mix & ~mask) | (src & mask);
                writeData(src ^ inverter);
            }
            
            if (align(x) < clip(x2))
                for (uint8_t ix = align(x); ix < clip(x2); ix += 8) {
                    setCursor(ix, y);
                    for (uint8_t iy = 0; iy < height; iy++) {
                        uint16_t src = pgm_read_byte(data + iy + stride)
                            + (data + iy < end? (pgm_read_byte(data + iy) << 8): 0);
                        src <<= align(x) - x;
                        src >>= 8;
                        writeData(src ^ inverter);
                    }
                    data += stride;
                }
            
            if (clip(x2) != x2) {
                mask = bitsBetween(clip(x2), x2);
                for (uint8_t iy = 0; iy < height; iy++) {
                    uint8_t mix = peek(clip(x2), y + iy);
                    uint16_t src = pgm_read_byte(data + iy + stride)
                        + (data + iy < end? (pgm_read_byte(data + iy) << 8): 0);
                    src <<= align(x) - x;
                    src >>= 8;
                    src = (mix & ~mask) | (src & mask);
                    writeData(src ^ inverter);
                }
            }
        }
    #else
        uint8_t y2 = y + height;
        if (clip(y) == clip(y2)) { // within one block
            uint8_t mask = bitsBetween(y, y2);
            for (uint8_t ix = 0; ix < width; ix++) {
                uint8_t mix = peek(x + ix, clip(y));
                uint8_t src = pgm_read_byte(data + ix);
                src <<= y % 8;
                src = (mix & ~mask) | (src & mask);
                writeData(src ^ inverter);
            }
        } else if (y == clip(y)) { // top is aligned
            for (uint8_t iy = y; iy < clip(y2); iy += 8) {
                setCursor(x, iy);
                for (uint8_t ix = 0; ix < width; ix++)
                    writeData(pgm_read_byte(data + ix) ^ inverter);
                data += stride;
            }
            if (y2 != clip(y2)) { // unaligned right
                uint8_t mask = bitsBetween(clip(y2), y2);
                for (uint8_t ix = 0; ix < width; ix++) {
                    uint8_t mix = peek(x + ix, clip(y2));
                    uint8_t src = pgm_read_byte(data + ix);
                    src = (mix & ~mask) | (src & mask);
                    writeData(src ^ inverter);
                }
            }
        } else { // left is unaligned
            uint8_t mask = bitsBetween(y, align(y));
            const uint8_t *end = data + stride * height;
            for (uint8_t ix = 0; ix < width; ix++) {
                uint8_t mix = peek(x + ix, clip(y));
                uint8_t src = pgm_read_byte(data + ix);
                src <<= y % 8;
                src = (mix & ~mask) | (src & mask);
                writeData(src ^ inverter);
            }
            
            if (align(y) < clip(y2))
                for (uint8_t iy = align(y); iy < clip(y2); iy += 8) {
                    setCursor(x, iy);
                    for (uint8_t ix = 0; ix < width; ix++) {
                        uint16_t src = pgm_read_byte(data + ix)
                            + (data + ix < end? (pgm_read_byte(data + ix + stride) << 8): 0);
                        src >>= align(y) - y;
                        writeData(src ^ inverter);
                    }
                    data += stride;
                }
            
            if (clip(y2) != y2) {
                mask = bitsBetween(clip(y2), y2);
                for (uint8_t ix = 0; ix < width; ix++) {
                    uint8_t mix = peek(x + ix, clip(y2));
                    uint16_t src = pgm_read_byte(data + ix)
                        + (data + ix < end? (pgm_read_byte(data + ix + stride) << 8): 0);
                    src >>= align(y) - y;
                    src = (mix & ~mask) | (src & mask);
                    writeData(src ^ inverter);
                }
            }
        }
    #endif
}

void Noritake_VFD_GUU100::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool on) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1? 1: -1;
    int sy = y0 < y1? 1: -1;
    int e = dx - dy;
    for (;;) {
        setDot(x0, y0, on);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * e;
        if (e2 > -dy) {
            e -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            e += dx;
            y0 += sy;
        }
    }
}

void Noritake_VFD_GUU100::drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool on) {
    uint8_t x2 = x + width;
    uint8_t y2 = y + height;
    drawLine(x, y, x2, y, on);
    drawLine(x2, y, x2, y2, on);
    drawLine(x, y2, x2, y2, on);
    drawLine(x, y, x, y2, on);
}

void Noritake_VFD_GUU100::smallRect(uint8_t x, uint8_t y, uint8_t len, uint8_t mask, bool on) {
    // This can be optimized by buffering.
    // Read all of the bytes into the buffer then, modify them.
    // Write out all of the bytes at once.
    #if NORITAKE_VERTICAL == 1
        while (len--) {
            uint8_t mix = peek(x, y++);
            mix = on? (mix | mask): (mix & ~mask);
            writeData(mix);
        }
    #else
        while (len--) {
            uint8_t mix = peek(x++, y);
            mix = on? (mix | mask): (mix & ~mask);
            writeData(mix);
        }
    #endif
}

void Noritake_VFD_GUU100::fillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool on) {
    if (invert) on = !on;
    if (x + width >= NORITAKE_WIDTH)
        width = NORITAKE_WIDTH - x;
    if (y + height >= NORITAKE_HEIGHT)
        height = NORITAKE_HEIGHT - y;
    if (width == 0 || height == 0)
        return;
            
    #if NORITAKE_VERTICAL == 1        
        uint8_t x2 = x + width;
        if (x / 8 == x2 / 8)
            smallRect(x, y, height, bitsBetween(x, x2), on);
        else {
            if (x % 8 != 0) {
                smallRect(x, y, height, bitsBetween(x, x2), on);
                x = align(x);
            }
            for (uint8_t ix = x; ix < clip(x2); ix += 8) {
                setCursor(ix, y);
                for (uint8_t i = height; i; i--)
                    writeData(on? 0xff: 0x00);
            }
            if (x2 % 8 != 0)
                smallRect(x2, y, height, bitsBetween(clip(x2), x2), on);
        }
    #else
        uint8_t y2 = y + height;
        if (y / 8 == y2 / 8)
            smallRect(x, y, width, bitsBetween(y, y2), on);
        else {
            if (y % 8 != 0) {
                smallRect(x, y, width, bitsBetween(y, y2), on);
                y = align(y);
            }
            for (uint8_t iy = y; iy < clip(y2); iy += 8) {
                setCursor(x, iy);
                for (uint8_t i = width; i; i--)
                    writeData(on? 0xff: 0x00);
            }
            if (y2 % 8 != 0)
                smallRect(x, y2, width, bitsBetween(clip(y2), y2), on);
        }
    #endif
}

void Noritake_VFD_GUU100::drawCircle(uint8_t cx, uint8_t cy, uint8_t radius, bool on) {
    int x = radius, y = 0, dx = 1 - 2 * radius, dy = 1, e = 0;
    while (x >= y) {
        setDot(cx+x, cy+y, on);
        setDot(cx-x, cy+y, on);
        setDot(cx-x, cy-y, on);
        setDot(cx+x, cy-y, on);
        setDot(cx+y, cy+x, on);
        setDot(cx-y, cy+x, on);
        setDot(cx-y, cy-x, on);
        setDot(cx+y, cy-x, on);
        y++;
        e += dy;
        dy += 2;
        if (2*e + dx > 0) {
            x--;
            e += dx;
            dx += 2;
        }
    }
}

void Noritake_VFD_GUU100::home() {
    setCursor(0, 0);
}
void Noritake_VFD_GUU100::carriageReturn() {
    setCursor(0, cur_y);
}
void Noritake_VFD_GUU100::lineFeed() {
    if (cur_y >= NORITAKE_HEIGHT - fontHeight)
        home();
    else
        setCursor(cur_x, cur_y + fontHeight);
}

void Noritake_VFD_GUU100::back() {
    if (cur_x < fontAdvance())
        if (cur_y < fontHeight)
            home();
        else
            setCursor(NORITAKE_WIDTH - fontAdvance(), cur_y - fontHeight);
    else
        setCursor(cur_x - fontAdvance(), cur_y);
}

void Noritake_VFD_GUU100::forward() {
    if (cur_x >= NORITAKE_WIDTH - fontAdvance())
        if (cur_y >= NORITAKE_HEIGHT - fontHeight)
            home();
        else
            crlf();
    else
        setCursor(cur_x + fontAdvance(), cur_y);
}

void Noritake_VFD_GUU100::crlf() {
    carriageReturn();
    lineFeed();
}

void Noritake_VFD_GUU100::setFont(const uint8_t *data, uint8_t width, uint8_t height) {
    font = data;
    fontWidth = width;
    fontHeight = height;
}

void Noritake_VFD_GUU100::setFont(const uint8_t *data, uint8_t width, uint8_t height, uint8_t gutter) {
    setFont(data, width, height);
    fontGutter = gutter;
}

void Noritake_VFD_GUU100::print(char c) {
    if (c < 0x20) {
        switch (c) {
        case '\b': back(); break;
        case '\t': forward(); break;
        case '\n': lineFeed(); break;
        case 0x0b: home(); break;
        case 0x0c: clearScreen(); break;
        case '\r': carriageReturn(); break;
        }
        return;
    }
    if (cur_x + fontAdvance() > NORITAKE_WIDTH)
        setCursor(0, cur_y + fontHeight >= NORITAKE_HEIGHT? 0: cur_y + fontHeight);
    if (cur_y + fontHeight > NORITAKE_HEIGHT)
        home();
    
    #if NORITAKE_VERTICAL == 1
        const uint8_t *p = font +
            (c - 0x20) * (fontWidth>8? align(fontWidth)/8: 1) * fontHeight;
    #else
        const uint8_t *p = font +
            (c - 0x20) * (fontHeight>8? align(fontHeight)/8: 1) * fontWidth;
    #endif
    
    uint8_t ox = cur_x;
    uint8_t oy = cur_y;
    drawImage(p, cur_x, cur_y, fontWidth, fontHeight);
    
    if (ox + fontAdvance() < NORITAKE_WIDTH) {
        fillRect(ox + fontWidth, oy, fontGutter, fontHeight, false);
        setCursor(ox + fontAdvance(), oy);
    }
    else {
        // No more characters in this font can fit on this line,
        // but move the cursor so that if a narrower font is
        // selected, this character won't be overwritten.
        setCursor(NORITAKE_WIDTH - 1, oy);
    }
}

void Noritake_VFD_GUU100::print(const char *str) {
    while (*str)
        print(*str++);
}

void Noritake_VFD_GUU100::print(const char *buffer, size_t size) {
    while (size--)
        print(*buffer++);
}

void Noritake_VFD_GUU100::print_p(const char *str) {
    while (pgm_read_byte(str))
        print(pgm_read_byte(str++));
}

void Noritake_VFD_GUU100::print_p(const char *buffer, size_t size) {
    while (size--)
        print(pgm_read_byte(buffer++));
}

void Noritake_VFD_GUU100::println(char c) {
    print(c);
    crlf();
}
void Noritake_VFD_GUU100::println(const char *str) {
    print(str);
    crlf();
}

void Noritake_VFD_GUU100::println(const char *buffer, size_t size) {
    print(buffer, size);
    crlf();
}

void Noritake_VFD_GUU100::println_p(const char *str) {
    print_p(str);
    crlf();
}

void Noritake_VFD_GUU100::println_p(const char *buffer, size_t size) {
    print_p(buffer, size);
    crlf();
}

bool Noritake_VFD_GUU100::print(int number, uint8_t max, char fill) {
    return print((long) number, max, fill);
}

bool Noritake_VFD_GUU100::print(unsigned number, uint8_t max, char fill) {
    return print((unsigned long) number, max, fill);
}

bool Noritake_VFD_GUU100::print(long number, uint8_t max, char fill) {
    bool neg = number < 0;
    char buf[10];
    if (neg)
        number = -number;
    if (!numberString(buf, number, max, fill))
        return false;
    if (neg) {
        uint8_t i;
        for (i = 0; i < max && buf[i] == fill; i++);
        if (i == 0) return false;
        buf[i-1] = '-';
    }
    print(buf, max);
    return true;
}

bool Noritake_VFD_GUU100::print(unsigned long number, uint8_t max, char fill) {
    char buf[10];
    if (!numberString(buf, number, max, fill))
        return false;
    print(buf, max);
    return true;
}

bool Noritake_VFD_GUU100::numberString(char buf[10], unsigned long number, uint8_t max, char fill) {
    if (max > 10) return false;
    for (uint8_t n = max; n--; number /= 10)
        buf[n] = (number || n==max-1)? number % 10 + '0': fill;
    return number == 0;
}

void Noritake_VFD_GUU100::print(int number, uint8_t base) {
    print((long)number, base);
}
void Noritake_VFD_GUU100::print(unsigned number, uint8_t base) {
    print((unsigned long)number, base);
}
void Noritake_VFD_GUU100::print(long number, uint8_t base) {
    if (number < 0) {
        print('-');
        number = -number;
    }
    print((unsigned long)number, base);
}
void Noritake_VFD_GUU100::print(unsigned long number, uint8_t base) {
    if (number / base)
        print(number / base, base);
    print(number % base < 10? number % base + '0': number % base - 10 + 'A');
}
