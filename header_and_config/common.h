/*
 * / LUR7.c - A collection of functions to setup and ease the use of the LUR7 PCB
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

#ifndef _COMMON_H_
#define _COMMON_H_

#define NBR_OF_IO 17

//digital inputs
#define IN1 0
#define IN2 1
#define IN3 2
#define IN4 3
#define IN5 4
#define IN6 5
#define IN7 6
#define IN8 7
#define IN9 8

//analog inputs
#define AN1 4
#define AN2 6

//digital outputs
#define OUT1 9
#define OUT2 10
#define OUT3 11
#define OUT4 12
#define OUT5 13
#define OUT6 14
#define OUT7 15
#define OUT8 16

// ADC
#define ADC_AN1 0x07
#define ADC_AN2 0x04
#define ADC_TEMP 0x0B
#define ADC_SUPPLY 0x10

void init_io(void);
void init_adc(void);

uint8_t set_output(uint8_t, uint8_t);
uint8_t get_output(uint8_t);
uint8_t toggle_output(uint8_t);

uint8_t get_input(uint8_t);
uint16_t get_analog(uint8_t);

#endif  // _COMMON_H_
