/*
 * LUR7_timer0.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file LUR7_timer0.h
 * \ref LUR7_timer0 sets up timer interrupts with a frequency of 100Hz for
 * the LUR7 project. All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the LUR7.h file to each source file.
 *
 * \addtogroup LUR7_timer0
 */

//see LUR7_timer0.c for documentation

#ifndef _LUR7_TIMER0_H_
#define _LUR7_TIMER0_H_

void timer0_init(void);

extern void timer0_isr_100Hz(uint8_t interrupt_nbr);

#endif //_LUR7_TIMER0_H_
