/*
 * ADCTest.c
 *
 * Created: 7/30/2013 8:21:53 PM
 *  Author: Aaron
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main(void)
{
	DDRD = 0x01; // D0 as output
	DDRB = 0x03; // B0 as output
	
	PORTD = 0x00;
	
	//ADC prescaler to 128 - 125kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	//set adc reference to AVCC, left adjust adc
	ADMUX |= (1 << REFS0) | (1 << ADLAR);
	
	
	//no MUX values need to be changed for ADC0
	
	//set ADC to free-running mode, enable ADC, enable ADC interrupt
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADATE);
		
	sei(); // enable global interrupts
	
	ADCSRA |= (1 << ADSC); // start ADC
	
    while(1)
    {
        PORTD = 0x01;
		_delay_ms(500);
		PORTD = 0x00;
		_delay_ms(500);
    }
}

ISR(ADC_vect)
{
	if ( ADCH <= 200 )
		PORTB = 0x00;
	else if (ADCH > 200 && ADCH <= 220)
		PORTB = 0x01;
	else if (ADCH > 220 && ADCH < 235)
		PORTB = 0x02;
	else
		PORTB = 0x03;
}