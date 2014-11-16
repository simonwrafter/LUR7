#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


uint32_t volatile overflowTimeCounter = 0;
uint32_t volatile timePWMInterruptUp = 0;
uint32_t volatile timePWMInterruptDown = 0;
uint32_t volatile tempTime = 0;

void setup();
void loop();

void main() {
	setup();
	while(1){
		loop();
	}
}

void setup()
{
 
    // initialize Timer1
    cli();             // disable global interrupts
    TCCR1A = 0;        // set entire TCCR1A register to 0
    TCCR1B = 0;
 
    // enable Timer1 overflow interrupt:
    TIMSK1 = (1 << TOIE1);
    // Set CS10 bit so timer runs at clock speed:
    TCCR1B |= (1 << CS11);



	DDRC = 1<<DDC3; // set pin 7 (PortC 3) as output.
	PCICR = 1<<PCIE0; // set pin change interrupt enable for pcint0:7 in Pin Change Interrupt Control Register
	PCMSK0 = 1<<PCINT2; // set pin change interrupt active for PCINT3 in Pin Change Mask Register 0
    PORTB = 1 << PORTB2; //Pull-up resistor är satt nu!!!!!!

    // enable global interrupts:
    sei();
}

void loop() {
	; // ain't done nuffin' or nuffin'
}

ISR(PCINT0_vect) {
	
	//PORTC = 1<<PORTC3; // Sätt på led


	if(PINB & 1<<PINB2){
		tempTime = overflowTimeCounter + TCNT1; //Varje bit är en mikrosekund nu!
		timePWMInterruptUp = tempTime;
		//PORTC = 1<<PORTC3; // Sätt på led

	} else {
		tempTime = overflowTimeCounter + TCNT1;
		uint32_t dutycycle = (tempTime - timePWMInterruptUp); //I mikrosekund
		//PORTC = 0x00; //Stäng av led
		
		if (dutycycle > 10000){
			PORTC = 1<<PORTC3; // Sätt på led
		} else{
			PORTC = 0x00; //Stäng av led
		}
		
	}

}

ISR(TIMER1_OVF_vect){
	overflowTimeCounter += 0x10000;
}
