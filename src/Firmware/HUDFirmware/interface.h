#include <avr/io.h>
#include <util/delay.h>

#define DIRECTION(X,D)  if (D) (*(&X##_PORT - 1) |= (1<<X##_PIN)); else (*(&X##_PORT - 1) &= ~(1<<X##_PIN))
#define RAISE(X)    X##_PORT |= (1<<X##_PIN)
#define LOWER(X)    X##_PORT &= ~(1<<X##_PIN)
#define CHECK(X)    (*(&X##_PORT - 2) & (1<<X##_PIN))
#define SETPIN(X,V)  if (V) RAISE(X); else LOWER(X)

void initPort();
void hardReset();
uint8_t readPort(bool rs, bool chip);
void writePort(uint8_t data, bool rs, bool chip);
