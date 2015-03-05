/*
 * / config.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

//Constants

//Inputs
//#define					IN1
//#define					IN2
#define IO_GP_BTN			IN3
#define IO_LOG_BTN			IN4
#define IO_GEAR_NEUTRAL		IN5
#define IO_CLUTCH		ADC_IN6
//#define					IN7
#define IO_GEAR_DOWN		IN8
#define IO_GEAR_UP			IN9

//Outputs
#define IO_SHIFT_STROBE		OUT2
#define IO_SHIFT_DATA		OUT3
#define IO_SHIFT_CLK		OUT4

//External interrupts
#define INT_GEAR_UP			INT_IN9_vect
#define INT_GEAR_DOWN		INT_IN8_vect
#define INT_GEAR_NEUTRAL	INT_IN5_vect

#endif // _CONFIG_H_
