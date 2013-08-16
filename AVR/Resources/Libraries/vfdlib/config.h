#include <avr/io.h>

//#define F_CPU               16000000UL // CPU speed in Hertz
#define NORITAKE_INTERFACE  0   // 0=Parallel, 1=Serial
#define NORITAKE_SERIAL     1   // Serial type (1=CU-UW, 2=SPI, 3=Signal Separate)
#define NORITAKE_VERTICAL   0   // 0=horizontal, 1=vertical
#define NORITAKE_WIDTH      (NORITAKE_VERTICAL? 64: 128)
#define NORITAKE_HEIGHT     (NORITAKE_VERTICAL? 128: 64)

#if NORITAKE_INTERFACE == 0     // Parallel interface
    #define DATA_PORT   PORTD
    #define RS_PIN      0
    #define RS_PORT     PORTA
    #define RW_PIN      1
    #define RW_PORT     PORTA
    #define E_PIN       2
    #define E_PORT      PORTA
    #define CS1_PIN     3
    #define CS1_PORT    PORTA
    #define CS2_PIN     4
    #define CS2_PORT    PORTA
    #define RESET_PIN   5    
    #define RESET_PORT  PORTA

#elif NORITAKE_INTERFACE == 1
    #if NORITAKE_SERIAL == 1     // Type 1: CU-UW
        #define SIO_PIN     4
        #define SIO_PORT    PORTG
        #define CS_PIN      7
        #define CS_PORT     PORTB
        #define SCK_PIN     3
        #define SCK_PORT    PORTG
        #define RESET_PIN   0
        #define RESET_PORT  PORTA
    #elif NORITAKE_SERIAL == 2 // Type 2: SPI
        #define SO_PIN      4
        #define SO_PORT     PORTG
        #define SCK_PIN     3
        #define SCK_PORT    PORTG
        #define RESET_PIN   0
        #define RESET_PORT  PORTA
        #define CS2_PIN     1
        #define CS2_PORT    PORTA
        #define CS1_PIN     2
        #define CS1_PORT    PORTA
        #define SI_PIN      3
        #define SI_PORT     PORTA
    #elif NORITAKE_SERIAL == 3 // Type 3: Signal Separate mode
        #define RS_PIN      5
        #define RS_PORT     PORTD
        #define SCK_PIN     4
        #define SCK_PORT    PORTD
        #define RESET_PIN   3
        #define RESET_PORT  PORTD
        #define CS2_PIN     2
        #define CS2_PORT    PORTD
        #define CS1_PIN     1
        #define CS1_PORT    PORTD
        #define SI_PIN      0
        #define SI_PORT     PORTD
    #endif
#endif
