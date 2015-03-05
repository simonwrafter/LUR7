/*
 * / midMCU.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

#ifndef _MIDMCU_H_
#define _MIDMCU_H_

#define REV_MIN 6000
#define REV_MAX 10500
#define REV_BAR_MIN 1
#define REV_BAR_MAX 22

#define TEMP_LVL_1 20
#define TEMP_LVL_2 40
#define TEMP_LVL_3 50
#define TEMP_LVL_4 60
#define TEMP_LVL_5 70
#define TEMP_LVL_6 80
#define TEMP_LVL_7 85
#define TEMP_LVL_8 90
#define TEMP_LVL_9 95
#define TEMP_LVL_10 100

void update_RPM(uint16_t);
void update_gear(uint8_t);
void update_speed(uint16_t);
void update_logid(uint16_t);
void update_watertemp(uint16_t);
void update_oiltemp(uint16_t);

uint8_t revs_to_bar(void);
uint8_t temp_to_bar(void);
void bcd_speed(void);
uint8_t bin_to_7seg(uint8_t, uint8_t);
void update_display(void);

#endif // _MIDMCU_H_
