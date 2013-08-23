/*
 * HUDFirmware.cpp
 *
 * Created: 8/18/2013 11:20:07 PM
 *  Author: Aaron
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
//include UART

#include "config.h"
#include "lcd.h"
#include "Noritake_VFD_GUU100.h"

//include images



int main(void)
{
	//setup IO
	//setup ADCs
	//setup interrupts
	//setup timers
	
	//initialize LCD
	//initialize VFD
	
    while(1)
    {
		//check mode
			//update if necessary
        //update LCD
			//output debugging info
		//update VFD
			//format data from RPi
    }
}

//ADC ISR
	//handle switch input
	//set modes/flags
	//photoresistor?

//UART ISR
	//handle data from RPi