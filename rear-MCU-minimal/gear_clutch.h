/*
 * gear_clutch.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

#ifndef _GEAR_CLUTCH_LAUNCH_H_
#define GEAR_CLUTCH_LAUNCH_H_

void gear_up(uint8_t);

void gear_down(uint8_t);

void gear_neutral(uint8_t);

void clutch_set(uint16_t);

void launch_control(void);

#endif // _GEAR_CLUTCH_LAUNCH_H_
