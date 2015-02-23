/*
 * LUR7_interrupt.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

#ifndef _LUR7_INTERRUPT_H_
#define _LUR7_INTERRUPT_H_

//interrupt vectors
//   external interrupts

#define INT_IN5_vect INT3_vect //PC0 - IN5
#define INT_IN8_vect INT2_vect //PB5 - IN8
#define INT_IN9_vect INT1_vect //PB2 - IN9

//interrupt functions
//   pin change interrupts

extern void pcISR_in1(void);
extern void pcISR_in2(void);
extern void pcISR_in3(void);
extern void pcISR_in4(void);
extern void pcISR_in5(void);
extern void pcISR_in6(void);
extern void pcISR_in7(void);
extern void pcISR_in8(void);
extern void pcISR_in9(void);

//EXTERNAL INTERRUPTS
uint8_t ext_int_on(uint8_t, uint8_t, uint8_t);
uint8_t ext_int_off(uint8_t);

//PIN CHANGE INTERRUPTS
void pc_int_on(uint8_t);
void pc_int_off(uint8_t);

//GENERAL
void interrupts_on(void);
void interrupts_off(void);

#endif // _LUR7_INTERRUPT_H_
