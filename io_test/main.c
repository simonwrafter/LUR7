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

static const uint8_t sev_seg[11] = {
	// a b c d e f g dp
	0b11111100,	//0
	0b01100000,	//1
	0b11011010,	//2
	0b11110010,	//3
	0b01100110,	//4
	0b10110110,	//5
	0b10111110,	//6
	0b11100000,	//7
	0b11111110,	//8
	0b11110110,	//9
	0b00000000	//blank
};

uint8_t bin_to_7seg(uint8_t binary, uint8_t dp) {
	if (binary >= 0 && binary <= 10) {
		if (!dp) {
			return sev_seg[binary];
		} else {
			return sev_seg[binary] + 1;
		}
	}
	return sev_seg[10];
}

void output_sev(uint8_t sev) {
	set_output(OUT1, sev & (1<<0)); //a
	set_output(OUT2, sev & (1<<1)); //b
	set_output(OUT3, sev & (1<<2)); //c
	set_output(OUT4, sev & (1<<3)); //d
	set_output(OUT5, sev & (1<<4)); //e
	set_output(OUT6, sev & (1<<5)); //f
	set_output(OUT7, sev & (1<<6)); //g
	set_output(OUT8, sev & (1<<7)); //dp
}

int main(void) {
	init_io();

	//pc_int_on(IN1);
	//pc_int_on(IN2);
	//pc_int_on(IN3);
	//pc_int_on(IN4);
	//pc_int_on(IN5);
	//pc_int_on(IN6);
	//pc_int_on(IN7);
	//pc_int_on(IN8);
	//pc_int_on(IN9);

	//interrupts_on();

	while (1) {
		output_sev(sev_seg[1]);
		_delay_ms(700);
		output_sev(sev_seg[2]);
		_delay_ms(700);
		output_sev(sev_seg[3]);
		_delay_ms(700);
		output_sev(sev_seg[4]);
		_delay_ms(700);
		output_sev(sev_seg[5]);
		_delay_ms(700);
		output_sev(sev_seg[6]);
		_delay_ms(700);
		output_sev(sev_seg[7]);
		_delay_ms(700);
		output_sev(sev_seg[8]);
		_delay_ms(700);
		output_sev(sev_seg[9]);
		_delay_ms(700);
		output_sev(sev_seg[10]);
		_delay_ms(700);
	}
	return 0;
}

void pcISR_in1(void) {
	set_output(OUT1, get_input(IN1));
}
void pcISR_in2(void) {
	set_output(OUT2, get_input(IN2));
}
void pcISR_in3(void) {
	set_output(OUT3, get_input(IN3));
}
void pcISR_in4(void) {
	set_output(OUT4, get_input(IN4));
}
void pcISR_in5(void) {
	set_output(OUT5, get_input(IN5));
}
void pcISR_in6(void) {
	set_output(OUT6, get_input(IN6));
}
void pcISR_in7(void) {
	set_output(OUT7, get_input(IN7));
}
void pcISR_in8(void) {
	set_output(OUT8, get_input(IN8));
}
void pcISR_in9(void) {
	set_output(OUT1, get_input(IN9));
}
