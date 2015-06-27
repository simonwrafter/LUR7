//**************************************************************
// ****** FUNCTIONS FOR SPI COMMUNICATION *******
//**************************************************************
//Controller: ATmega32 (Clock: 8 Mhz-internal)
//Compiler	: AVR-GCC (winAVR with AVRStudio)
//Project V.: Version - 2.4.1
//Author	: CC Dharmani, Chennai (India)
//			  www.dharmanitech.com
//Date		: 24 Apr 2011
//**************************************************************

#include <avr/io.h>
#include "SPI_routines.h"

//SPI initialize for SD card
//clock rate: 125Khz
void SPI_init(void) {
	DDRB |= (1 << DDB1) | (1 << DDB7); // MOSI & SCK to output
	
	//setup SPI: Interrupts enabled, Master mode, MSB first, SCK phase low, SCK idle low
	SPCR = (1 << SPIE) | (1 << SPE) | (1 << MSTR) | (1 << SPR1);
	SPSR = (1 << SPI2X);
}

void SPI_send_byte(uint8_t data) {
	SPDR = data;
}

unsigned char SPI_receive(void) {
	unsigned char data;
	// Wait for reception complete
	SPDR = 0xff;
	while (!(SPSR & (1<<SPIF))) {
		;
	}
	data = SPDR;
	// Return data register
	return data;
}

void SPI_high_speed(void) {
	SPCR &= 0xFC; //clear clock bits, set SPI clock to 4 MHz
}

ISR(SPI_STC_vect) {
	
}