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

/*! \file LUR7_timer1.c
 * \ref LUR7_timer1 generates a high precision PWM signal on \ref OUT1 at a 
 * frequency of 400Hz.
 * 
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 *
 * \see LUR7_timer1
 * \see LUR7_timer1.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 * 
 * \defgroup LUR7_timer1 Shared - Timer 1
 * Timer 1 generates a high precision PWM signal on \ref OUT1 at a frequency of
 * 400Hz. The dutycycle can be set to any value from 0 to 20000.
 *
 * \see LUR7_timer1.c
 * \see LUR7_timer1.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "LUR7.h"
#include "LUR7_timer1.h"

//! Hardware initialisation function.
/*!
 * The timer is setup up in phase and frequency correct PWM mode with OC1B
 * (OUT1) as output. The PWM frequency is calculated as
 * \f[f_{PWM} = \frac {f_{IO}}{2 \times N \times TOP} = \frac {16000000}{2 \times 1 \times 20000} = 400.000 Hz\f]
 * TOP is defined in Output Compare Register 1A (OCR1A) as 20000 meaning the 
 * frequency is 400Hz.
 */
void timer1_init(void) {
	TCCR1A = (1 << COM1B1) | (1 << WGM10); // non-inverting output on PC1 (OC1B), phase and frequency correct PWM mode
	TCCR1B = (1 << WGM13) | (1 << CS10); // phase and frequency correct PWM mode, prescaler = 1
	TCCR1C = 0x00; // no force compare match
	OCR1A  = 0x4E20; // 20000 => 400Hz
	OCR1B  = 0x0000; // duytcycle = 0 to start
	TIMSK1 = 0x00; // no interrupts
}

//! Sets the dutycycle of the output.
/*!
 * The dutycycle of the output is controlled by Output Compare Register 1B. The
 * output is toggled upon a match between the compare register value and the TOP
 * value in Output Compare Register 1A. As TOP=20000 \p dutycycle can be any 
 * number [0, 20000].
 * 
 * \param dutycycle the dutycycle [0, 20000] of the PWM output.
 */
void timer1_dutycycle(uint16_t dutycycle) {
	if (dutycycle > 20000) {
		dutycycle = 20000;
	}
	OCR1B = dutycycle;
}
