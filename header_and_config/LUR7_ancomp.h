/*
 * LUR7_ancomp.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file LUR7_ancomp.h
 * \ref LUR7_ancomp is used to detect Brown Out conditions as early as possible.
 * 
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 *
 * \see LUR7_ancomp
 * \see LUR7_ancomp.c
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 * 
 * \addtogroup LUR7_ancomp
 */

#ifndef _LUR7_ANCOMP_H_
#define _LUR7_ANCOMP_H_

void ancomp_init(void);

//! Early Brown Out warning.
/*!
 * This function is executed when a brown out condition occures.
 */
extern void early_bod_warning_ISR(void);

//! Early Brown Out warning over.
/*!
 * This function is executed when the voltage on the 12V mains is higher than 
 * that of the capacitor bank.
 * 
 * \warning \ref early_bod_safe_ISR does not indicate that the Brown Out is over,
 * merely that it has stabilised at a some voltage level. To be sure of the 
 * running conditions also use the \ref LUR7_adc to measure the external voltage.
 */
extern void early_bod_safe_ISR(void);

#endif // _LUR7_ANCOMP_H_
