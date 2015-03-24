/*
 * config.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

//Constants
#define CLUTCH_ATOMIC_LENGTH	10

//Inputs

// Outputs
// Outout for Shift Register Strobe
#define IO_SHIFT_STROBE		OUT5
// Outout for Shift Register Data
#define IO_SHIFT_DATA		OUT6
// Outout for Shift Register Clock
#define IO_SHIFT_CLK		OUT7

//External interrupts

#endif // _CONFIG_H_
