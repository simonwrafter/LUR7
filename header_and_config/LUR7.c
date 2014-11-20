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

/*
 * #define ADPS0 0
 * #define ADPS1 1
 * #define ADPS2 2
 #define ADIE 3
 #define ADIF 4
 #define ADATE 5
 #define ADSC 6
 #define ADEN 7
 */
void init_adc(void) {
	ADMUX = (1<<REFS0) | (1<<ADLAR) | (1<<MUX3) | (1<<MUX1) | (1<<MUX0);
	ADCSRA = 
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

uint16_t get_analog(uint8_t port, uint8_t ) {
	if (port)
}