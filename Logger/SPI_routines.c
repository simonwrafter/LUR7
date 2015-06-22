#include <avr/io.h>
#include <avr/stdint.h>
#include "SPI_routines.h"

//SPI initialize for SD card
//clock rate: 125Khz
void SPI_init(void) {
	DDRD |= (1 << DDD3) | (1 << DDD4); // MOSI_A & SCK_A to output
	DDRC |= (1 << DDC1); // SS_A as output
	
	//setup SPI: select alternate port, Interrupts enabled, Master mode, MSB first, SCK phase low, SCK idle low
	MCUCR |= (1 << SPIPS);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
	SPSR = (1 << SPI2X);
}

void SPI_send_uint8_t(uint8_t data) {
	SPDR = data;
	while (!(SPSR & (1<<SPIF))) {
		;
	}
}

uint8_t SPI_receive(void) {
	uint8_t data;
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

void SPI_select(void) {
	PORTC |= (1 << PORTC1);
	set_output(OU1);
}

void SPI_deselect(void) {
	PORTC &= ~(1 << PORTC1);
}
