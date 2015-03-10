/*
 * LUR7_timer0.c - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file LUR7_timer0.c
 * \ref LUR7_timer0 sets up timer interrupts with a frequency of 100Hz for
 * the LUR7 project.
 * 
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 *
 * \see LUR7_timer0
 * \see LUR7_timer0.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 * 
 * \defgroup LUR7_timer0 Shared - Timer 0
 * Timer0 is set up in CTC mode generating timer interrupts at a frequency of
 * 100Hz. Thus making it possible to schedule tasks and repeat them with at
 * known time intervalls. No output is connected to the timer
 *
 * \see LUR7_timer0.c
 * \see LUR7_timer0.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "LUR7.h"
#include "LUR7_timer0.h"

//! Identifies interrupt
/*! interrupt_nbr is used to identify which interrupt in the 100Hz cycle occured
 * last. Passed on to extern \ref timer0_isr_100Hz function.
 */
static volatile uint8_t interrupt_nbr = 0;

//! Hardware initialisation function.
/*!
 * Initialises Timer0. No outputs are connected to the output comparator
 * registers, and the counter is cleared on timer compare (CTC mode). Prescaling
 * the clock by 1024 and setting the compare register to 155 results in a
 * interrupt frequency of 100 Hz per
 * \f[f = \frac {f_{IO}}{N \times (OCR0A+1)} = \frac {16000000}{1024 \times (155 + 1)} = 100.160 Hz\f]
 */
void timer0_init(void) {
	TCCR0A = (1 << WGM01); // no outputs, CTC mode
	TCCR0B = (1 << CS02) | (1 << CS00); // prescaler = 1024
	OCR0A  = 0b11111011; // OCR0A = 155 => 100,160 Hz,
	/* OCR0A  = 0b11111011; // OCR0A = 251 => 62,004 Hz,
	 * OCR0A  = 0b11011001; // OCR0A = 217 => 72,005 Hz.
	 * OCR0A  = 0b10011011; // OCR0A = 155 => 100,160 Hz,
	 * OCR0A  = 0b01111101; // OCR0A = 125 => 124,008 Hz.
	 * OCR0A  = 0b01001101; // OCR0A = 77 => 200,320 Hz.
	 */
	TIMSK0 = (1 << OCIE0A); // interrupt enable for OCR0A
}

//! Interrupt Service Routine, Timer0
/*!
 * Interrupts are triggered every 10ms, the counter \ref interrupt_nbr is
 * incresed each time and cycled modulo 100. The extern function
 * \ref timer0_isr_100Hz is called and can be used for scheduling tasks.
 */
ISR(TIMER0_COMPA_vect) {
	timer0_isr_100Hz(interrupt_nbr++);
	if (interrupt_nbr >= 100) {
		interrupt_nbr = 0;
	}
}
