/*
 * LCDTesting.cpp
 *
 * Created: 8/4/2013 12:41:30 PM
 *  Author: Aaron
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lcd.h"

#define STRING_LENGTH 28
#define LCD_MAX 20

int brightness = 100;
char buffer[10];
bool update = true;

int main(void)
{
	//ADC prescaler to 128 - 125kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	//set adc reference to AVCC, left adjust adc
	ADMUX |= (1 << REFS0) | (1 << ADLAR);
	
	//no MUX values need to be changed for ADC0
	
	//enable ADC, enable ADC interrupt
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADATE);
	
	sei(); // enable global interrupts
	
	ADCSRA |= (1 << ADSC); // start ADC
	
	//timer stuff
	TCCR1B |= (1 << CS10) | (1 << CS11);
	
     lcd_init(LCD_DISP_ON); /* initialize lcd, display on, cursor on */
                                  /* for more options for */
                                  /* lcd_init(), view lcd.h file */
								  
	lcd_home();
	lcd_puts("Brightness: ");
	lcd_gotoxy(0, 2);
	lcd_puts("ADC: ");
    while(1)                      /* run continuously */
    {
		if (update)
		{  
			itoa(brightness, buffer, 10);
			lcd_gotoxy(12, 0);
			lcd_puts("   ");
			lcd_gotoxy(12, 0);
			lcd_puts(buffer);        /* type something random */
			lcd_gotoxy(5,2);          /* go to 2nd row 1st col */
			lcd_puts("   ");
			lcd_gotoxy(5,2);  
			itoa(ADCH, buffer, 10);
			lcd_puts(buffer);        /* type something random */
			update = false;
		}
        _delay_ms(5);            /* wait 50ms */
		
    }
}

ISR(ADC_vect)
{
	if ( ADCH <= 208 )
	{
		if (TCNT1 > 20000)
		{
			TCCR1B = 0x00;
			TCNT1 = 0;
		}
	}
	else if (ADCH > 208 && ADCH <= 225)
	{
		if (TCNT1 > 20000)
		{
			if (brightness >= 22)
			{
				brightness -= 13;
				update = true;
			}
			TCCR1B = 0x00;
			TCNT1 = 0;
		}
		else
		{
			if ((TCCR1B & CS10) != 1) TCCR1B |= (1 << CS10) | (1 << CS11);
		}
	}
	else if (ADCH > 225 && ADCH < 238)
	{
		if (TCNT1 > 20000)
		{
			if (brightness < 88)
			{
				brightness += 13;
				update = true;
			}
			TCCR1B = 0x00;
			TCNT1 = 0;
		}
		else
		{
			if ((TCCR1B & CS10) != 1) TCCR1B |= (1 << CS10) | (1 << CS11);
		}
	}
}