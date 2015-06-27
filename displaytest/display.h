/*
 * display.h - Controls the LED display of the LUR7
 * Copyright (C) 2014  Simon Wrafter <simon.wrafter@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file display.h
 * \ref display controls the LED display used in the LUR7.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref display.c
 * \see \ref main.c
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \addtogroup display
 */

#ifndef _MIDMCU_H_
#define _MIDMCU_H_

//! Water Temperature for one lit LED
#define TEMP_LVL_1 20
//! Water Temperature for two lit LED
#define TEMP_LVL_2 40
//! Water Temperature for three lit LED
#define TEMP_LVL_3 50
//! Water Temperature for four lit LED
#define TEMP_LVL_4 60
//! Water Temperature for five lit LED
#define TEMP_LVL_5 70
//! Water Temperature for six lit LED
#define TEMP_LVL_6 80
//! Water Temperature for seven lit LED
#define TEMP_LVL_7 85
//! Water Temperature for eight lit LED
#define TEMP_LVL_8 90
//! Water Temperature for nine lit LED
#define TEMP_LVL_9 95
//! Water Temperature for ten lit LED (max)
#define TEMP_LVL_10 100

void update_RPM(uint16_t);
void update_gear(uint8_t);
void update_speed(uint16_t);
void update_logid(uint16_t);
void update_watertemp(uint16_t);
void update_oiltemp(uint16_t);

uint8_t get_current_gear(void);

uint8_t revs_to_bar(void);
uint8_t temp_to_bar(void);
void bcd_convert(uint16_t);
uint8_t bin_to_7seg(uint8_t, uint8_t);
void update_display(void);

#endif // _MIDMCU_H_
