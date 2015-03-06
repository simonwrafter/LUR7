/*
 * / config.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

//Constants
#define BRAKES_ON			100

//Inputs
#define WHEEL_R				IN1
#define WHEEL_L				IN2
//#define					IN3
#define BRAKE_PRESSURE	ADC_IN4
//#define					IN5
#define STEERING_WHEEL	ADC_IN6
//#define					IN7
#define SUSPENSION_R	ADC_IN8
#define SUSPENSION_L	ADC_IN9

//Outputs
//none

//External interrupts
//none

#endif // _CONFIG_H_
