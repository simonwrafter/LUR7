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
#include "LUR7_timer1.h"

//static volatile uint8_t interrupt_nbr = 0;

void timer1_init(void) { //(uint8_t pwm_on) {
	//if (pwm_on) {
		TCCR1A = (1 << COM1B1) | (1 << WGM10); // non-inverting output on PC1 (OC1B), phase and frequency correct PWM mode
	//} else {
	//	TCCR1A = (1 << WGM10); // no pwm output on PC1 (OC1B)
	//}
	TCCR1B = (1 << WGM13) | (1 << CS10); // phase and frequency correct PWM mode, prescaler = 1
	TCCR1C = 0x00; // no force compare match
	OCR1A  = 0x4E20; // 20000 => 400Hz
	OCR1B  = 0x0000;
	ICR1   = 0x0000;
	TIMSK1 = 0x00; //(1 << OCIE1A);
}

void set_pwm_output(uint16_t dutycycle) {
	if (dutycycle > 20000) {
		dutycycle = 20000;
	}
	OCR1B = dutycycle;
}

/*
ISR(TIMER1_COMPA_vect) {
	timer1_isr_400Hz(interrupt_nbr++);
	if (interrupt_nbr == 400) {
		interrupt_nbr = 0;
	}
}
*/
