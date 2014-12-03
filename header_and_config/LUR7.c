/*
/ LUR7.c - A collection of functions to setup and ease the use of the LUR7 PCB
/ Copyright (C) 2014  Simon Wrafter <simon.wrafter@gmail.com>
/ 
/ This program is free software: you can redistribute it and/or modify
/ it under the terms of the GNU General Public License as published by
/ the Free Software Foundation, either version 3 of the License, or
/ (at your option) any later version.
/ 
/ This program is distributed in the hope that it will be useful,
/ but WITHOUT ANY WARRANTY; without even the implied warranty of
/ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/ GNU General Public License for more details.
/ 
/ You should have received a copy of the GNU General Public License
/ along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


void init_io(void) {
	int i;
	for (i=OUT1; i<NBR_OF_IO; i++) { //set DDXn=1 for all outputs
		DDX[i] |= (1<<DDXn);
	}
}

void init_adc(void) {
	ADMUX = (1<<REFS0) | (1<<MUX3) | (1<<MUX1) | (1<<MUX0);
	ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);
	ASCSRB = (1<<ADHSM);
}

uint8_t set_output(uint8_t port, uint8_t data) {
	if (data) {
		PORTX[port] |= (1 << PORTXn[port]);
	} else {
		PORTX[port] &= ~(1 << PORTXn[port]);
	}
}

uint8_t get_output(uint8_t port) {
	return get_input(port);
}

uint8_t toggle_output(uint8_t){
	PINX[port] = (1 << PINXn[port]);
}

uint8_t get_input(uint8_t port) {
	return (PINX[port] & _BV(PINXn[port]) ? 1 : 0);
}

uint16_t get_analog(uint8_t analog_port) {
	ADMUX &= ~((1<<MUX4) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
	ADMUX |= analog_port;
	ADCSRA = (1<<ADSC);
	
	while (!(ADCSRA & (1<<ADIF))) {
		;
	}
	ADCSRA |= (1<<ADIF);
	return (ADCH<<8) | ADCL;
}
