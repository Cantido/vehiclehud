#include <stdint.h>
#include <stddef.h>

class Noritake_VFD_GUU100 {

protected:
    void increment();    
    void smallRect(uint8_t x, uint8_t y, uint8_t width, uint8_t mask, bool on);
public:
    #if NORITAKE_VERTICAL == 1
        unsigned cur_y:7;
        unsigned display_on:1;
        unsigned cur_x:6;
    #else
        unsigned cur_x:7;
        unsigned display_on:1;
        unsigned cur_y:6;
    #endif
    unsigned power_save:1;
    unsigned brightness:3;
    unsigned fontGutter:3;
    unsigned invert:1;

    const uint8_t *font;
    uint8_t fontWidth;
    uint8_t fontHeight;

    static uint8_t align(uint8_t v) { return (v + 7) / 8 * 8; }
    static uint8_t clip(uint8_t v) { return v / 8 * 8; }
    uint8_t fontAdvance() { return this->fontWidth + this->fontGutter; }
    uint8_t peek(uint8_t x, uint8_t y) {
        if (x >= NORITAKE_WIDTH || y >= NORITAKE_HEIGHT)
            return 0;
        setCursor(x, y);
        uint8_t data = readData();
        setCursor(x, y);
        return data;
    }

    void displayOn();
    void displayOff(bool powerSave);
    void reset();
    void setScreenBrightness(uint8_t percent);

    void invertOn();
    void invertOff();

    void init();
    void writeData(uint8_t data);
    void command(uint8_t bits, bool chip);
    uint8_t readData();
    uint8_t readStatus(bool chip);
    void setCursor(uint8_t x, uint8_t y);

    void back();
    void forward();
    void lineFeed();
    void home();
    void clearScreen();
    void carriageReturn();
    void crlf();

    void setDot(uint8_t x, uint8_t y, bool on);
    void drawImage(const uint8_t *data, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
    void drawImage(const uint8_t *data, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t stride);
    void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool on);
    void drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool on);
    void fillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool on);
    void drawCircle(uint8_t cx, uint8_t cy, uint8_t radius, bool on);

    void setFont(const uint8_t *data, uint8_t width, uint8_t height);
    void setFont(const uint8_t *data, uint8_t width, uint8_t height, uint8_t gutter);
    void print(char c);
    void print(const char *str);
    void print(const char *buffer, size_t size);
    void print_p(const char *str);
    void print_p(const char *buffer, size_t size);
    void println(char c);
    void println(const char *str);
    void println(const char *buffer, size_t size);
    void println_p(const char *str);
    void println_p(const char *buffer, size_t size);

    bool numberString(char buf[10], unsigned long number, uint8_t max, char fill);
    bool print(int number, uint8_t max, char fill);
    bool print(unsigned number, uint8_t max, char fill);
    bool print(long number, uint8_t max, char fill);
    bool print(unsigned long number, uint8_t max, char fill);

    void print(int number, uint8_t base);
    void print(unsigned number, uint8_t base);
    void print(long number, uint8_t base);
    void print(unsigned long number, uint8_t base);
};
