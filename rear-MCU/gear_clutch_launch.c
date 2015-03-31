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

/*! \file gear_clutch_launch.c
 * \ref gear_clutch_launch contains functions for gear changing, clutch and 
 * launch control on the rear MCU used in the LUR7.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref gear_clutch_launch
 * \see \ref gear_clutch_launch.h
 * \see \ref main.c
 * \see \ref config.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup gear_clutch_launch Rear MCU - Gear, Clutch and Launch Control
 * \ref gear_clutch_launch.c controls gear changing procedures, clutch 
 * positioning and launch control.
 *
 * \see \ref gear_clutch_launch.c
 * \see \ref gear_clutch_launch.h
 * \see \ref main
 * \see \ref config
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "../header_and_config/LUR7.h"
#include "gear_clutch_launch.h"
#include "config.h"

//! Set to TRUE when system is working.
static volatile uint8_t busy = FALSE;

//! Delay between engaging shift cut and running the solenoid.
static const uint16_t SHIFT_CUT_DELAY = 1000;
//! Time to run the solenoid for gear up.
static const uint16_t GEAR_UP_DELAY = 1000;
//! Time to run the solenoid for gear down.
static const uint16_t GEAR_DOWN_DELAY = 1000;
//! Time to run the solenoid for neutral up.
static const uint16_t NEUTRAL_UP_DELAY = 1000;
//! Time to run the solenoid for neutral down.
static const uint16_t NEUTRAL_DOWN_DELAY = 1000;
//! Time to run the signal for launch control
static const uint16_t LAUNCH_SIGNAL_DELAY = 500;

//! Number of cluych readings to average
static const uint16_t CLUTCH_AVERAGE_NUMBER = 10;
//! Symmetry point, the resting value of the clutch position sensor.
static const float CLUTCH_BIAS_MID = 512;
//! Threshold value for closed clutch
static const float CLUTCH_POS_CLOSED = 650;
//! Threshold value for open clutch
static const float CLUTCH_POS_OPEN = 900;
//! PWM value for closed clutch
static const float CLUTCH_DC_CLOSED = 4000;
//! PWM value for open clutch
static const float CLUTCH_DC_OPEN = 12000;

//! Array holding the \ref CLUTCH_AVERAGE_NUMBER number of readings to average.
volatile static float clutch_old = 0;
static const float clutch_factor = 0.1;

//! Pointer to function call after time elapses.
static void (*volatile end_fun_ptr)(void);
//! Function to start second part of gear up.
static void mid_gear_up(void);
//! Function to end gear up routine.
static void end_gear_change(void);
//! Launch Control, end signal
static void end_launch_signal(void);

//! Change gear up, part 1
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
 *   <li> set \ref busy flag.
 *   <li> start shift cut by setting \ref SHIFT_CUT to GND.
 *   <li> set \ref end_fun_ptr to \ref mid_gear_up for second part of routine.
 *   <li> start timer0, \ref SHIFT_CUT_DELAY.
 *   </ol>
 * <li> In \ref timer0_isr_stop, call \ref mid_gear_up.
 *   <ol>
 *   <li> turn off \ref SHIFT_CUT signal, the actual shift cut will be dissengaged by the DTA.
 *   <li> run solenoid on \ref GEAR_UP.
 *   <li> set \ref end_fun_ptr to \ref end_gear_change for last part of routine.
 *   <li> start timer0, \ref GEAR_UP_DELAY.
 *   </ol>
 * <li> In \ref timer0_isr_stop, call \ref end_gear_change.
 *   <ol>
 *   <li> turn of solenoid.
 *   <li> clear busy flag.
 *   </ol>
 * </ol>
 * 
 * The flag \ref busy is set at the start of each operation and cleared once 
 * timer0 finishes, hindering more than one action at a time. If \ref busy is set
 * when the function is triggered no gear change will happen.
 * 
 * If the car is in fifth gear, trying to shift up will have no effect. However,
 * this is only a feature as long as the DTA is not in fail mode.
 * 
 * \todo special case for neutral -> 1st.
 * 
 * \param current_gear allows the controler to behave in the most appropriate manner
 */
void gear_up(uint8_t current_gear) {
	if (!busy && current_gear != 5) {
		busy = TRUE;
		set_output(SHIFT_CUT, GND);
		end_fun_ptr = mid_gear_up;
		timer0_start(SHIFT_CUT_DELAY);
	}
}

//! Change gear up, part 2
/*!
 * turn off signal for shift cut and engage solenoid, delay for \ref GEAR_UP_DELAY
 * time. end_fun_ptr set to \ref end_gear_change.
 */
static void mid_gear_up(void) {
	set_output(SHIFT_CUT, TRI); // reset shift cut output
	set_output(GEAR_UP, GND); // run solenoid
	end_fun_ptr = end_gear_change;
	timer0_start(GEAR_UP_DELAY); // new delay for actual gear change
}

//! End gear change sequence
/*!
 * Turns off the solenoid in both directions. Frees the busy flag.
 */
static void end_gear_change(void) {
	set_output(GEAR_UP, TRI); // reset output
	set_output(GEAR_DOWN, TRI); // reset output
	busy = FALSE; // free unit
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
 * 
 * If the car is in first gear, trying to shift down will have no effect. However,
 * this is only a feature as long as the DTA is not in fail mode.
 * 
 * \param current_gear allows the controler to behave in the most appropriate manner
 */
void gear_down(uint8_t current_gear) {
	if (!busy && current_gear != 1) {
		busy = TRUE;
		set_output(GEAR_DOWN, GND);
		end_fun_ptr = end_gear_change;
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
 * Should the DTA be in fail mode the neutral finder will be active in all gears,
 * it will assume being in first gear and try to find neutral above the current gear.
 * 
 * All delay times are static and no adaptive scaling of the force to find the
 * neutral is implemented.
 * 
 * \param current_gear allows the controler to behave in the most appropriate manner
 */
void gear_neutral(uint8_t current_gear) {
	if (!busy) {
		if (current_gear == 1 || current_gear == 11) {
			busy = TRUE;
			set_output(GEAR_UP, GND);
			end_fun_ptr = end_gear_change;
			timer0_start(NEUTRAL_UP_DELAY);
		} else if (current_gear == 2) {
			busy = TRUE;
			set_output(GEAR_DOWN, GND);
			end_fun_ptr = end_gear_change;
			timer0_start(NEUTRAL_DOWN_DELAY);
		}
	}
}

//! Position the clutch servo.
/*!
 * The sensor positioned in the steering wheel givves an analog signal which is 
 * converted to a 10 bit digital number (0 - 1023). As the clutch paddle moves 
 * in both directions the position of the clutch is calculated as the displacement 
 * from the mid-point (512). To avoid having the servo move due to any vibrations 
 * that find their way in to the steering wheel the paddle must be moved by a 
 * minimum threshold angle. A maximum value also exists to prevent over extension 
 * of the clutch. Between the lower threshold and maximum values a linear mapping 
 * function transforms the ADC value to a useable PWM dutycycle for the servo.
 * 
 * \todo possibly divide the range into three zones.
 * 
 * \param pos the angle of the clutch paddle.
 */
void clutch_set(uint16_t pos) {

	volatile float clutch_new = clutch_factor * pos + (1 - clutch_factor) * clutch_old;
	clutch_old = clutch_new;

	if (clutch_new < CLUTCH_BIAS_MID) {
		clutch_new = 1024 - clutch_new;
	}
	if (clutch_new < CLUTCH_POS_CLOSED) {
		timer1_dutycycle(CLUTCH_DC_CLOSED);
	} else if (clutch_new > CLUTCH_POS_OPEN) {
		timer1_dutycycle(CLUTCH_DC_OPEN);
	} else {
		timer1_dutycycle(((clutch_new - CLUTCH_POS_CLOSED) * ((CLUTCH_DC_OPEN - CLUTCH_DC_CLOSED)) / (CLUTCH_POS_OPEN - CLUTCH_POS_CLOSED) + CLUTCH_DC_CLOSED));
	}
}

//! Launch Control signal
/*!
 * Sets a signal to the DTA that initiates its launch control system.
 */
void launch_control(void) {
	if (!busy) {
		busy = TRUE;
		set_output(LAUNCH, GND);
		end_fun_ptr = end_launch_signal;
		timer0_start(LAUNCH_SIGNAL_DELAY);
	}
}

//! Launch Control, end signal
/*!
 * stops the signal to the DTA when Launch Control is active.
 */
static void end_launch_signal(void) {
	set_output(LAUNCH, TRI);
	busy = FALSE;
}

//! Timed function.
/*!
 * Used for ending gear change routines. Uses the function pointer \ref end_fun_ptr
 * to call the correct next function.
 */
void timer0_isr_stop(void) {
	(*end_fun_ptr)();
}
