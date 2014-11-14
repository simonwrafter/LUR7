#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define DataPort 	PORTC
#define DataIn		PORTB
#define PinOut		PORTC3
#define PinIn		PORTB2

void main() {
	setup();
	while(1){
		loop();
	}
}

void setup() {
	DDRC = 0xFF; 	// set pin 7 (PortB 3) as output.
	DataPort = 0xFF;
	DataIn = 0x00;
	ADMUX = 0x65; 	//Sätter ADMUX till "0110 0101", 
					//01 = Vref
	   				//1 = Left adjust
	   				//0 0101 = 5 alltså ADC5, PORTB2	
	ADCSRA = 0xBF; 	//BF = 1011 1111
	ADCSRB = 0x00;
	sei();
	ADCSRA |= 1<<ADSC;

}

void loop() {
	;
}

ISR(ADC_vect) { 
	_delay_ms(500);
	DataPort = ADCH;
	ADCSRA |= 1<<ADSC;



}