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

int main(void) {
	init_io();

	pc_int_on(IN6);
	//pc_int_on(IN2);
	//pc_int_on(IN3);
	//pc_int_on(IN4);
	//pc_int_on(IN5);
	//pc_int_on(IN6);
	//pc_int_on(IN7);
	//pc_int_on(IN8);
	//pc_int_on(IN9);

	interrupts_on();

	while (1) {
		;
	}
	return 0;
}

void pcISR_in1(void) {
	set_output(OUT1, get_input(IN1));
}
void pcISR_in2(void) {
	set_output(OUT1, get_input(IN2));
}
void pcISR_in3(void) {
	set_output(OUT1, get_input(IN3));
}
void pcISR_in4(void) {
	set_output(OUT1, get_input(IN4));
}
void pcISR_in5(void) {
	set_output(OUT1, get_input(IN5));
}
void pcISR_in6(void) {
	set_output(OUT1, get_input(IN6));
}
void pcISR_in7(void) {
	set_output(OUT1, get_input(IN7));
}
void pcISR_in8(void) {
	set_output(OUT1, get_input(IN8));
}
void pcISR_in9(void) {
	set_output(OUT1, get_input(IN9));
}
