/*
 * / main.c - A collection of functions to setup and ease the use of the LUR7 PCB
 * / Copyright (C) 2015  Simon Wrafter <simon.wrafter@gmail.com>
 * /
 * / This program is free software: you can redistribute it and/or modify
 * / it under the terms of the GNU General Public License as published by
 * / the Free Software Foundation, either version 3 of the License, or
 * / (at your option) any later version.
 * /
 * / This program is distributed in the hope that it will be useful,
 * / but WITHOUT ANY WARRANTY; without even the implied warranty of
 * / MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * / GNU General Public License for more details.
 * /
 * / You should have received a copy of the GNU General Public License
 * / along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../header_and_config/LUR7.h"
#include "rearMCU.h"

uint32_t volatile overflowTimeCounter = 0;
uint32_t volatile velocity_l = 0;
uint32_t volatile velocity_r = 0;


int main(void) {
	init_io();
	init_adc();
	init_interrupt();
	TCCR1A = 0;        		// set entire TCCR1A register to 0
    TCCR1B = 0;
    TIMSK1 = (1 << TOIE1);  // enable Timer1 overflow interrupt
    TCCR1B |= (1 << CS12);  // Set CS12 prescaler (100). Prescaler set to 256. 16 000 000 CPU speed / 256 = 62 500. Each bit is 1/62500 s = 16 us
    interrupts_on();
	while (1) {
		update_analog(); // Better to call from a timer interrupt, perhaps together with CAN send
	}
	return 0;
}

//With each bit change 16 us, TIMER1 will overflow each 1,048576 s
ISR(TIMER1_OVF_vect){
	overflowTimeCounter += 1;
	velocity_l = speed_l*0.04;
}
