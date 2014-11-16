#include <avr/io.h>
#include <avr/interrupt.h>

void main() {
	setup();
	while(1){
		loop();
	}
}

void setup() {
	DDRC = 1<<DDC3; // set pin 7 (PortB 3) as output.
	PCICR = 1<<PCIE0; // set pin change interrupt enable for pcint0:7 in Pin Change Interrupt Control Register
	PCMSK0 = 1<<PCINT3; // set pin change interrupt active for PCINT3 in Pin Change Mask Register 0
	sei(); // set global interrupt active, I bit in SREG
}

void loop() {
	; // ain't done nuffin' or nuffin'
}

ISR(PCINT0_vect) { // activated if there is change to PCINT0
	if (PINB & _BV(PINB3)) { // if PINB3 is high after interrupt
		PINC = 1<<PINC3; // toggle output to PINC3
	}
}
