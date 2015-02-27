/*
/ LUR7_adc.c - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file LUR7_adc.c
 * LUR7_adc.c sets up and retreives analog input values through 10 bit adc for
 * the LUR7 project. All code is released under the GPLv3 license.
 * 
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the LUR7.h file to your main.c file.
 * 
 * In this file functions pertaining to the initialization and usage of the 10
 * bit ADC are defined.
 */

#include "LUR7.h"
#include "LUR7_adc.h"

void adc_init(void) {
	ADMUX = (1<<REFS0) | (1<<MUX3) | (1<<MUX1) | (1<<MUX0);
	ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);
	ADCSRB = (1<<ADHSM);
	DIDR0 = (1 << ADC2D) | (1 << ADC3D);
}

uint16_t adc_get(uint8_t analog_port) {
	ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
	ADMUX |= analog_port;
	if (analog_port < 8) {
		DIDR0 |= (1 << analog_port);
	}
	ADCSRA |= (1<<ADSC);

	while ((ADCSRA & (1<<ADSC))) {
		;
	}

	if (analog_port < 8) {
		DIDR0 &= ~(1 << analog_port);
	}
	return ADCL | (ADCH<<8);
}
