/*
 * / shiftregister.c - A collection of functions to setup and ease the use of the LUR7 PCB
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
#include "config.h"
#include "shiftregister.h"

void shift_byte(uint8_t value) {
	for (uint8_t i= 0; i<8; i++) {
		shift_bit(value & (1<<i));
	}
}

void shift_bar(uint8_t led_on, uint8_t length) {
	uint8_t led_off = length - led_on;
	for (uint8_t on=0; on<led_on; on++) {
		shift_bit(1);
	}
	for (uint8_t off=0; off<led_off; off++) {
		shift_bit(0);
	}
}

void shift_bit(uint8_t value) {
	set_output(SHIFT_DATA, (value>0));
	_delay_us(1);
	set_output(SHIFT_CLK, 1);
	_delay_us(1);
	set_output(SHIFT_CLK, 0);
}

void shift_strobe(void) {
	set_output(SHIFT_STROBE, 1);
	_delay_us(1);
	set_output(SHIFT_STROBE, 0);
}
