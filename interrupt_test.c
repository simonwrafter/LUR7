/*
 *  Written in AVR Studio 5 / AVR Studio 6
 *  Compiler: AVR GNU C Compiler (GCC)
 *
 *  Author: AVR Tutorials
 *  Website: www.AVR-Tutorials.com
*/
 
#include <avr/io.h>
#include <avr/interrupt.h>
 

//Ändra F_CPU
//#define F_CPU 4000000UL
#include <util/delay.h>
 
#define DataPort	PORTC	// Using PortC as our Dataport
#define DataDDR		DDRC
 
//Interrupt Service Routine for INT0
ISR(INT0_vect)
{
	unsigned char i, temp;
 
	_delay_ms(500); // Software debouncing control
 
	//temp = DataPort;	// Save current value on DataPort
 
	/* This for loop blink LEDs on Dataport 5 times*/
	for(i = 0; i<100; i++)
	{
		DataPort = 0x00;
		_delay_ms(50);	// Wait 5 seconds
		DataPort = 0x01;
		_delay_ms(50);	// Wait 5 seconds
	}
 
	//DataPort = temp;	//Restore old value to DataPort	
}	
 
int main(void)
{
	PD2 Får nog ändras först!
	DDRD = 1<<PD2;		// Set PD2 as input (Using for interupt INT0)
	PORTD = 1<<PD2;		// Enable PD2 pull-up resistor
 
	DataDDR = 0x01;		// Configure Dataport as output // Nu bara pin 0
	DataPort = 0x01;	// Initialise Dataport to 1
 
	GICR = 1<<INT0;					// Enable INT0
	MCUCR = 1<<ISC01 | 1<<ISC00;	// Trigger INT0 on rising edge
 
	sei();				//Enable Global Interrupt
 
    while(1)
    {
		if(DataPort == 0x01)
			DataPort = 0x00;	// Släck dioden
		else
			DataPort = 0x01;	// Tänd dioden
 
		_delay_ms(1000);	// Wait 1 seconds
    }
}