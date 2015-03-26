/*
 * gear_clutch.c - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file gear_clutch.c
 * \ref gear_clutch contains functions for gear changing and clutch control on
 * the rear MCU used in the LUR7.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref gear_clutch
 * \see \ref gear_clutch.h
 * \see \ref main.c
 * \see \ref config.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup gear_clutch Rear MCU - Gear and Clutch Control
 * \ref gear_clutch.c controls gear changing procedure and clutch positioning.
 *
 * \see \ref gear_clutch.c
 * \see \ref gear_clutch.h
 * \see \ref main
 * \see \ref config
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "../header_and_config/LUR7.h"
#include "gear_clutch.h"
#include "config.h"

//! Set to TRUE when gear change system is working.
static volatile uint8_t busy = FALSE;
//! Set to TRUE during the shift cut delay.
static volatile uint8_t shift_cut_flag = FALSE;
//! Set to TRUE during the gear up sequence.
static volatile uint8_t gear_up_flag = FALSE;
//! Set to TRUE during the gear down sequence.
static volatile uint8_t gear_down_flag = FALSE;
//! Set to TRUE during the neutral-from-first-gear sequence.
static volatile uint8_t neutral_up_flag = FALSE;
//! Set to TRUE during the neutral-from-second-gear sequence.
static volatile uint8_t neutral_down_flag = FALSE;

//! Delay between engaging shift cut and running the solenoid.
static const uint16_t SHIFT_CUT_DELAY = 1000;
//! time to run the solenoid for gear up.
static const uint16_t GEAR_UP_DELAY = 1000;
//! time to run the solenoid for gear down.
static const uint16_t GEAR_DOWN_DELAY = 1000;
//! time to run the solenoid for neutral up.
static const uint16_t NEUTRAL_UP_DELAY = 1000;
//! time to run the solenoid for neutral down.
static const uint16_t NEUTRAL_DOWN_DELAY = 1000;

//! Threshold value for closed clutch
static const uint16_t CLUTCH_POS_CLOSED = 600;
//! Threshold value for open clutch
static const uint16_t CLUTCH_POS_OPEN = 900;
//! PWM value for closed clutch
static const uint16_t CLUTCH_DC_CLOSED = 4000;
//! PWM value for open clutch
static const uint16_t CLUTCH_DC_OPEN = 9000;


//! Change gear up
/*!
 * Changing up a gear is the most intricate of the gear changing routines. Shift 
 * cut needs to be applied and allowed to take effect before the solenoid is 
 * engaged. There being two long delays to consider in this routine means 
 * the more straight forward method used in \ref gear_down is not applicable.
 * 
 * The steps to change gear are thus:
 * <ol>
 * <li> In \ref gear_up function.
 *   <ol>
 *   <li> set \ref busy flag, and \ref shift_cut_flag.
 *   <li> start shift cut by setting \ref SHIFT_CUT to GND.
 *   <li> start timer0, \ref SHIFT_CUT_DELAY.
 *   </ol>
 * <li> In \ref timer0_isr_stop with \ref shift_cut_flag set.
 *   <ol>
 *   <li> clear \ref shift_cut_flag.
 *   <li> turn off \ref SHIFT_CUT signal, the actual shift cut will be dissengaged by the DTA.
 *   <li> set \ref gear_up_flag.
 *   <li> run solenoid on \ref GEAR_UP.
 *   <li> start timer0, \ref GEAR_UP_DELAY.
 *   </ol>
 * <li> In \ref timer0_isr_stop with \ref gear_up_flag set.
 *   <ol>
 *   <li> clear \ref gear_up_flag.
 *   <li> turn of solenoid.
 *   <li> clear busy flag.
 *   </ol>
 * </ol>
 * 
 * The flag \ref busy is set at the start of each operation and cleared once 
 * timer0 finishes, hindering more than one action at a time. If \ref busy is set
 * when the function is triggered no gear change will happen.
 */
void gear_up(uint8_t current_gear) {
	if (!busy && current_gear < 5) {
		busy = TRUE;
		shift_cut_flag = TRUE;
		set_output(SHIFT_CUT, GND);
		timer0_start(SHIFT_CUT_DELAY);
	}
}

//! Change gear down
/*!
 * This function assumes that the clutch is engaged when triggered. The flag 
 * \ref busy is set at the start of each operation and cleared once timer0 
 * finishes, hindering more than one action at a time. If \ref busy is set when
 * the function is triggered no gear change will happen. 
 * 
 * The procedure is straight forward, \ref gear_down_flag is set and the 
 * \ref GEAR_DOWN output set to activate the solenoid for the time defined in
 * \ref GEAR_DOWN_DELAY. Once \ref timer0_isr_stop is triggered, the output is 
 * reset and the flags cleared.
 */
void gear_down(uint8_t current_gear) {
	if (!busy && current_gear > 1) {
		busy = TRUE;
		gear_down_flag = TRUE;
		set_output(GEAR_DOWN, GND);
		timer0_start(GEAR_DOWN_DELAY);
	}
}

//! Find neutral gear
/*!
 * This function assumes that the clutch is engaged when triggered. The flag 
 * \ref busy is set at the start of each operation and cleared once timer0 
 * finishes, hindering more than one action at a time. If \ref busy is set when
 * the function is triggered no gear change will happen.
 * 
 * The procedure is fairly straight forward, depending on the currently selected 
 * gear (1 or 2, others disregarded) the solenoid is set to give a pulse in the
 * appropriate direction. \ref neutral_up_flag (from gear 1) or 
 * \ref neutral_down_flag (from gear 2) is set and the \ref GEAR_UP (1) or 
 * \ref GEAR_DOWN (2) output set to activate the solenoid for the time defined in
 * either \ref NEUTRAL_UP_DELAY (1) or \ref NEUTRAL_DOWN_DELAY (2). Once 
 * \ref timer0_isr_stop is triggered, the output is reset and the flags cleared.
 * 
 * All delay times are static and no adaptive scaling of the force to find the
 * neutral is implemented.
 */
void gear_neutral(uint8_t current_gear) {
	if (!busy) {
		if (current_gear == 1) {
			busy = TRUE;
			neutral_up_flag = TRUE;
			set_output(GEAR_UP, GND);
			timer0_start(NEUTRAL_UP_DELAY);
		} else if (current_gear == 2) {
			busy = TRUE;
			neutral_down_flag = TRUE;
			set_output(GEAR_DOWN, GND);
			timer0_start(NEUTRAL_DOWN_DELAY);
		}
	}
}

//! Position the clutch servo.
/*!
 * not implemented
 */
void clutch_set(uint16_t pos) {
	if (pos < 512) {
		pos = 1024 - pos;
	}
	if (pos < CLUTCH_POS_CLOSED) {
		timer1_dutycycle(CLUTCH_DC_CLOSED);
	} else if (pos > CLUTCH_POS_OPEN) {
		timer1_dutycycle(CLUTCH_DC_OPEN);
	} else {
		timer1_dutycycle((pos - CLUTCH_POS_CLOSED) * (CLUTCH_DC_OPEN - CLUTCH_DC_CLOSED) / (CLUTCH_POS_OPEN - CLUTCH_POS_CLOSED) + CLUTCH_DC_CLOSED);
	}
}

/*!
 * Used for ending gear change routines.
 */
void timer0_isr_stop(void) {
	if (shift_cut_flag) { // shift cut is used for changing up a gear
		shift_cut_flag = FALSE; // clear flag
		set_output(SHIFT_CUT, TRI); // reset shift cut output
		gear_up_flag = TRUE; // set gear up flag
		set_output(GEAR_UP, GND); // run solenoid
		timer0_start(GEAR_UP_DELAY); // new delay for actual gear change
	} else if (gear_up_flag) {
		gear_up_flag = FALSE; // clear flag
		set_output(GEAR_UP, TRI); // reset output
		busy = FALSE; // free unit
	} else if (gear_down_flag) {
		gear_down_flag = FALSE; // clear flag
		set_output(GEAR_DOWN, TRI); // reset output
		busy = FALSE; // free unit
	} else if (neutral_up_flag) { // from 1
		neutral_up_flag = FALSE; // clear flag
		set_output(GEAR_UP, TRI); // reset output
		busy = FALSE; // free unit
	} else if (neutral_down_flag) { // from 2
		neutral_down_flag = FALSE; // clear flag
		set_output(GEAR_DOWN, TRI); // reset output
		busy = FALSE; // free unit
	}
}
