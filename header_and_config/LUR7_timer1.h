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

/*! \file LUR7_timer1.h
 * \ref LUR7_timer1 
 * 
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 *
 * \see LUR7_timer1
 * \see LUR7_timer1.c
 * \see http://www.gnu.org/copyleft/gpl.html
 * 
 * \addtogroup LUR7_timer1
 */

#ifndef _LUR7_TIMER1_H_
#define _LUR7_TIMER1_H_

void timer1_init(void);
void timer1_dutycycle(uint16_t);

// extern void timer1_isr_400Hz(uint8_t);

#endif //_LUR7_TIMER1_H_
