/*
 * LCDTesting.cpp
 *
 * Created: 8/4/2013 12:41:30 PM
 *  Author: Aaron
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

int main(void)
{
	DDRB |= 0x01;
	_delay_ms(1000);
	
	PORTB |= 0x01;
	_delay_ms(1000);
	
    lcd_init(LCD_DISP_ON);						  
	
	PORTB &= 0xFE;							  

	lcd_home();
	lcd_puts("8===D");
    while(1)
    {
        _delay_ms(500);
		PORTB |= 0x01;
		_delay_ms(500);
		PORTB &= 0xFE;
    }
}