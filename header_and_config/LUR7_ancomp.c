/*
 * LUR7_ancomp.c - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file LUR7_ancomp.c
 * \ref LUR7_ancomp is used to detect Brown Out conditions as early as possible.
 * 
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 *
 * \see LUR7_ancomp
 * \see LUR7_ancomp.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * 
 * \defgroup LUR7_ancomp Analog Comparator
 * By connecting the 12V main power supply and capacitor bank to either input of
 * an Analog Comparator and generating interrupts the 12v main supply dips below
 * the capacitor pack, early Brown Out conditions can be identified and handled.
 * 
 * \see LUR7_ancomp.c
 * \see LUR7_ancomp.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 */

#include "LUR7.h"
#include "LUR7_ancomp.h"

//! Hardware initialisation function.
/*!
 * Analog Comparator 2 is configured to generate interrupts on output toggle,
 * comparing the positive and negative input for the comparator.
 */
void ancomp_init(void) {
	AC2CON = (1 << AC2EN) | (1 << AC2IE) | (1 << AC2M2) | (1 << AC2M1); //enable interrupts on output toggle, compare ACMP2 and ACMP2N.
	DIDR0 = (1 << ADC2D) | (1 << ADC3D); //turn of digital buffers for used inputs
}

//! Interrupt Service Routine, Analog Comparator 2.
/*!
 * When a output toggle occurs this Interrupt Service Routine is triggered. The
 * output of the comparator is checked to see what state it is in determining 
 * whether to execute \ref early_bod_warning_ISR or \ref early_bod_safe_ISR.
 * 
 * \warning \ref early_bod_safe_ISR does not indicate that the Brown Out is over,
 * merely that it has stabilised at a some voltage level. To be sure of the 
 * running conditions also use the \ref LUR7_adc to measure the external voltage.
 */
ISR(ANACOMP2_vect) {
	if (ACSR & (1 << AC2O)) { // see if output is high
		early_bod_warning_ISR(); //Warn
	} else {
		early_bod_safe_ISR(); // "Safe", poor indicator as the actual condition of the 12V mains are unknown. this only indicates that the BOD has stabilised.
	}
}
