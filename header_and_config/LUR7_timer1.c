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
 * frequency of 400Hz and timer interrupts at 100Hz.
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
 * 400Hz. The dutycycle can be set to any value from 0 to 20000. Also a 100 Hz 
 * timer interrupt is generated.
 *
 * \see LUR7_timer1.c
 * \see LUR7_timer1.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "LUR7.h"
#include "LUR7_timer1.h"

//! Identifies interrupt
/*! Interrupt_nbr is used to identify which interrupt in the 100Hz cycle occured
 * last. Passed on to extern \ref timer0_isr_100Hz function.
 */
static volatile uint8_t interrupt_nbr = 0;
//! Interrupt counter 'prescaler'
/*!
 * timer interrupts are triggered at 400Hz, but dividev by 4 to generate 100Hz 
 * interrupts.
 */
static volatile uint8_t interrupt_divider = 0;

//! Hardware initialisation function.
/*!
 * The timer is setup up in phase and frequency correct PWM mode with OC1B
 * (OUT1) as output. The PWM frequency is calculated as
 * \f[f_{PWM} = \frac {f_{IO}}{2 \times N \times TOP} = \frac {16000000}{2 \times 1 \times 20000} = 400.000 Hz\f]
 * TOP is defined in Output Compare Register 1A (OCR1A) as 20000 meaning the 
 * frequency is 400Hz.
 * 
 * Interrupts are enabled for TOP, generating a steady 400 Hz timer interrupt.
 * This is divided by 4 and used to call the extern function \ref timer1_isr_100Hz.
 * 
 * \param pwm_on wheter to output pwm on OUT1 or not.
 */
void timer1_init(uint8_t pwm_on) {
	if (pwm_on) {
		TCCR1A = (1 << COM1B1) | (1 << WGM10); // non-inverting output on PC1 (OC1B), phase and frequency correct PWM mode
	} else {
		TCCR1A = (1 << WGM10); // no pwm output on PC1 (OC1B), normal operation.
	}
	
	TCCR1B = (1 << WGM13) | (1 << CS10); // phase and frequency correct PWM mode, prescaler = 1
	TCCR1C = 0x00; // no force compare match
	OCR1A  = 0x4E20; // 20000 => 400Hz
	OCR1B  = 0x0000; // duytcycle = 0 to start
	TIMSK1 = (1 << OCIE1A); // timer interrupts 400Hz
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

//! Interrupt Service Routine, Timer1
/*!
 * Interrupts are triggered every 10ms, the counter \ref interrupt_nbr is
 * incresed each time and cycled modulo 100. The extern function
 * \ref timer1_isr_100Hz is called and can be used for scheduling tasks.
 */
ISR(TIMER1_COMPA_vect) {
	interrupt_divider = (interrupt_divider + 1) % 4;
	if (interrupt_divider == 0) {
		timer1_isr_100Hz(interrupt_nbr++);
		if (interrupt_nbr == 100) {
			interrupt_nbr = 0;
		}
	}
}
