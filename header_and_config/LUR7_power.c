/*
 / LUR7_power.c - A coll*ection of functions to setup and ease the use of the LUR7 PCB
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

#include "LUR7.h"
#include "LUR7_power.h"

void power_off_default(void) {
	PRR = (1 << PRPSC) | (1 << PRSPI) | (1 << PRLIN);
}

void power_off_timer1(void) {
	PRR |= (1 << PRTIM1);
}
