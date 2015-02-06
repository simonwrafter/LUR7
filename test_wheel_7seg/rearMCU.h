/*
 * / rearMCU.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

#ifndef _REARMCU_H_
#define _REARMCU_H_

#define WHEEL_SPEED_L	IN5 //INT3
#define WHEEL_SPEED_R	IN9 //INT1, (also AN4, not used)
#define SUSPENSION_L	ADC_AN1 //IN4
#define SUSPENSION_R	ADC_AN2 //IN6

void init_interrupt(void);
void update_analog(void);
void update7seg(void);
uint8_t bin_to_7seg(uint8_t, uint8_t);
void blink (uint8_t);

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

#endif  // _REARMCU_H_
