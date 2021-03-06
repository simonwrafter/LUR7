/*
 * gear_clutch_launch.h - A collection of functions to setup and ease the use of the LUR7 PCB
 * Copyright (C) 2015  Simon Wrafter <simon.wrafter@gmail.com>
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

/*! \file gear_clutch_launch.h
 * The \ref gear_clutch_launch defines inputs and constants used on the rear MCU.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref gear_clutch_launch
 * \see \ref gear_clutch_launch.c
 * \see \ref main.c
 * \see \ref config.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \addtogroup gear_clutch_launch
 */

#ifndef _CLUTCH_H_
#define _CLUTCH_H_

void clutch_init(void);
void clutch_filter_left(uint16_t pos_left);
void clutch_filter_right(uint16_t pos_right);
void clutch_dutycycle_left(void);
void clutch_dutycycle_right(void);
uint16_t clutch_get_filtered_left(void);
uint16_t clutch_get_filtered_right(void);
uint16_t clutch_get_dutycycle_left(void);
uint16_t clutch_get_dutycycle_right(void);
void clutch_set_dutycycle(void);

#endif // _CLUTCH_H_
