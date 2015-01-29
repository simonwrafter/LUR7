/*
 * / frontMCU.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

#ifndef _FRONTMCU_H_
#define _FRONTMCU_H_

#define WHEEL_SPEED_L	IN5
#define WHEEL_SPEED_R	IN9 //also analog port AN4
#define SUSPENSION_L	ADC_AN1 //IN4
#define SUSPENSION_R	ADC_AN2 //IN6
#define STEERING_WHEEL	ADC_AN3 //IN8

void init_interrupt(void);
void update_analog(void);

#endif  // _FRONTMCU_H_
