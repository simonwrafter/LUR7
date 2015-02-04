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
#include "config.h"
#include "midMCU.h"
#include "shiftregister.h"

int main(void) {
	init_io();
	init_adc();
	uint8_t val = 7;
	uint8_t seg = 0;

	while (1) {
		seg = bin_to_7_seg(val);
		set_output(OUT1, seg & (1<<0));
		set_output(OUT2, seg & _BV(1));
		set_output(OUT3, seg & _BV(2));
		set_output(OUT4, seg & _BV(3));
		set_output(OUT5, seg & _BV(4));
		set_output(OUT6, seg & _BV(5));
		set_output(OUT7, seg & _BV(6));
		set_output(OUT8, seg & _BV(7));
	}
	return 0;
}

