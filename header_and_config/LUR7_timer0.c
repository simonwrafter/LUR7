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
 * \ref LUR7_timer0 Interrupt based delay functionality.
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
 * Timer0 provides a way of generating timed delays that are not busy-wait based.
 * By using timed interrupts and counters time can be measured with a resolution 
 * of 100µs.
 *
 * \see LUR7_timer0.c
 * \see LUR7_timer0.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "LUR7.h"
#include "LUR7_timer0.h"

static volatile uint16_t counter = 0;
static volatile uint16_t compare = 0;

//! Hardware initialisation function.
/*!
 * Initialises Timer0. CTC mode, timer is stoped. To start a delay use 
 * \ref timer0_start, when the specified time has elapsed \ref timer0_stop 
 * executes.
 */
void timer0_init(void) {
	TCCR0A = (1 << WGM01); //no output, CTC mode
	TCCR0B = 0x00; // timer stoped.
	OCR0A = 199; // 100µs with clock prescaler = 8.
}

//! Start delay
/*!
 * Resets counter and starts a timed delay. after \p time has elapsed 
 * \ref timer0_stop executes. \p time can be given with a resolution of 100µs.
 * 
 * \param time time in ms*10 (100µs resolution)
 */
void timer0_start(uint16_t time) {
	TCNT0 = 0x00; // reset counter.
	TCCR0B |= (1 << CS01); // clock prescaler = 8.
	TIMSK0 = (1 << OCIE0A); // enable output compare interrupt.
	compare = time; // store time to pause.
}

//! Interrupt Service Routine, Timer0
/*!
 * Interrupt handler. Handles the timer and counts the time passed. Executes 
 * \ref timer0_stop when \p time has elapsed and stops the timer.
 */
ISR(TIMER0_COMPA_vect) {
	if (++counter == compare) {
		TCCR0B = 0; // turn off counter
		TIMSK0 = 0; // disable interrupts
		compare = 0; // reset compare.
		timer0_isr_stop();
	}
}
