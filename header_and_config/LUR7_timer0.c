/*
 * LUR7_timer0.c - A collection of functions to setup and ease the use of the LUR7 PCB
 * Copyright (C) 2014  Simon Wrafter <simon.wrafter@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "LUR7.h"
#include "LUR7_timer0.h"

static volatile uint8_t interrupt_nbr = 0;

void timer0_init(void) {
	TCCR0A = (1 << WGM01); // no outputs, CTC mode
	TCCR0B = (1 << CS02) | (1 << CS00); // prescaler = 1024
	OCR0A  = 0b11111011; // OCR0A = 155 => 50,080 Hz,
	/* OCR0A  = 0b11111011; // OCR0A = 251 => 31,00198 Hz,
	 * OCR0A  = 0b11011001; // OCR0A = 217 => 36,0023 Hz.
	 * OCR0A  = 0b10011011; // OCR0A = 155 => 50,080 Hz,
	 * OCR0A  = 0b01111101; // OCR0A = 125 => 62,004 Hz.
	 * OCR0A  = 0b01001101; // OCR0A = 77 => 100,160 Hz.
	 */
	TIMSK0 = (1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect) {
	timer0_isr_100Hz(interrupt_nbr++);
	if (interrupt_nbr >= 50) {
		interrupt_nbr = 0;
	}
}
