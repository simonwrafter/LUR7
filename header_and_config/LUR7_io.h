/*
 * LUR7_io.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file LUR7_io.h
 * \ref LUR7_io provides functions for setting up and using the digital inputs
 * and outputs of the LUR7 PCB. 
 * 
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 *
 * \see LUR7_io
 * \see LUR7_io.c
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \addtogroup LUR7_io
 */

#ifndef _LUR7_IO_H_
#define _LUR7_IO_H_

void io_init(void);

uint8_t set_output(uint8_t, uint8_t);
uint8_t get_output(uint8_t);
uint8_t toggle_output(uint8_t);

uint8_t get_input(uint8_t);

#endif  // _LUR7_IO_H_
