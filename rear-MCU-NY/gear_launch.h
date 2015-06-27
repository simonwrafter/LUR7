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
 * The \ref gear_launch defines inputs and constants used on the rear MCU.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref gear_launch
 * \see \ref gear_launch.c
 * \see \ref main.c
 * \see \ref config.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \addtogroup gear_clutch_launch
 */

#ifndef _GEAR_CLUTCH_LAUNCH_H_
#define _GEAR_CLUTCH_LAUNCH_H_

#define POT_FAIL 11

void set_current_gear(uint8_t gear);
uint8_t get_current_gear(void);
void set_current_revs(uint16_t);

void gear_up(void);
void gear_down(void);

void gear_neutral_single(void);
void gear_neutral_repeat_linear(void);
void gear_neutral_repeat_bisect(void);

//void launch_control(void);
//void launch_stop_clutch(void);

#endif // _GEAR_CLUTCH_LAUNCH_H_
