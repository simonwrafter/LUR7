/*
 * / midMCU.c - A collection of functions to setup and ease the use of the LUR7 PCB
 * / Copyright (C) 2014  Simon Wrafter <simon.wrafter@gmail.com>
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
#include "midMCU.h"

void midMCU_main() {
	volatile uint16_t revs = 0;
	volatile uint16_t gear = 0;
	volatile uint16_t speed = 0;
	volatile uint16_t log_id = 0;
	volatile uint16_t temp = 0;
	
	
	
	while (1) {
		;
	}
}

