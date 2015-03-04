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

/*! \file LUR7_interrupt.h
 * \ref LUR7_interrupt provides functions for setting up and using input
 * interrupts on the LUR7 PCB. 
 * 
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 *
 * 
 * \see LUR7_interrupt
 * \see LUR7_interrupt.c
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * 
 * \addtogroup LUR7_interrupt
 */

#ifndef _LUR7_INTERRUPT_H_
#define _LUR7_INTERRUPT_H_

//interrupt vectors
//   external interrupts

//! H
#define INT_IN5_vect INT3_vect //PC0 - IN5
//! H
#define INT_IN8_vect INT2_vect //PB5 - IN8
//! H
#define INT_IN9_vect INT1_vect //PB2 - IN9

//interrupt functions
//   pin change interrupts
//! Pin Change Interrupt handler for IN1.
/*!
 * If including this through \ref LUR7.h this function will need to be defined
 * in the code. It will be executed when a Pin Change Interrupt occures on IN1.
 */
extern void pcISR_in1(void);
//! Pin Change Interrupt handler for IN2.
/*!
 * If including this through \ref LUR7.h this function will need to be defined
 * in the code. It will be executed when a Pin Change Interrupt occures on IN2.
 */
extern void pcISR_in2(void);
//! Pin Change Interrupt handler for IN3.
/*!
 * If including this through \ref LUR7.h this function will need to be defined
 * in the code. It will be executed when a Pin Change Interrupt occures on IN3.
 */
extern void pcISR_in3(void);
//! Pin Change Interrupt handler for IN4.
/*!
 * If including this through \ref LUR7.h this function will need to be defined
 * in the code. It will be executed when a Pin Change Interrupt occures on IN4.
 */
extern void pcISR_in4(void);
//! Pin Change Interrupt handler for IN5.
/*!
 * If including this through \ref LUR7.h this function will need to be defined
 * in the code. It will be executed when a Pin Change Interrupt occures on IN5.
 */
extern void pcISR_in5(void);
//! Pin Change Interrupt handler for IN6.
/*!
 * If including this through \ref LUR7.h this function will need to be defined
 * in the code. It will be executed when a Pin Change Interrupt occures on IN6.
 */
extern void pcISR_in6(void);
//! Pin Change Interrupt handler for IN7.
/*!
 * If including this through \ref LUR7.h this function will need to be defined
 * in the code. It will be executed when a Pin Change Interrupt occures on IN7.
 */
extern void pcISR_in7(void);
//! Pin Change Interrupt handler for IN8.
/*!
 * If including this through \ref LUR7.h this function will need to be defined
 * in the code. It will be executed when a Pin Change Interrupt occures on IN8.
 */
extern void pcISR_in8(void);
//! Pin Change Interrupt handler for IN9.
/*!
 * If including this through \ref LUR7.h this function will need to be defined
 * in the code. It will be executed when a Pin Change Interrupt occures on IN9.
 */
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
