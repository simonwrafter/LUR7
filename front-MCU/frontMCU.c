/*
 * / frontMCU.c - A collection of functions to setup and ease the use of the LUR7 PCB
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
#include "frontMCU.h"

static volatile uint16_t speed_l = 0;
static volatile uint16_t speed_r = 0;
static volatile uint16_t susp_l = 0;
static volatile uint16_t susp_r = 0;
static volatile uint16_t steering = 0;

void init_interrupt(void) {
	EICRA = (1<<ISC30) | (1<<ISC10); //logical change to INT1 and INT3
	EIMSK = (1<<INT1) | (1<<INT3);
}

void update_analog(void) {
	susp_l = get_analog(SUSPENSION_L);
	susp_r = get_analog(SUSPENSION_R);
	steering = get_analog(STEERING_WHEEL);
}

ISR(INT3_vect) { // SPEED_L
	;
}

ISR(INT1_vect) { // SPEED_R
	;
}
