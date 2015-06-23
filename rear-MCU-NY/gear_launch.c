/*
 * gear_clutch_launch.c - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file gear_launch.c
 * \ref gear_launch contains functions for gear changing, clutch and
 * launch control on the rear MCU used in the LUR7.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref gear_launch
 * \see \ref gear_launch.h
 * \see \ref main.c
 * \see \ref config.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup gear_clutch_launch Rear MCU - Gear, Clutch and Launch Control
 * \ref gear_launch.c controls gear changing procedures, clutch
 * positioning and launch control.
 *
 * \see \ref gear_launch.c
 * \see \ref gear_launch.h
 * \see \ref main
 * \see \ref config
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "../header_and_config/LUR7.h"
#include "gear_launch.h"
#include "config.h"


//********** COMMON ************************************************************

//! Set to TRUE when system is working.
static volatile uint8_t busy = FALSE;
//! Currently selected gear as read by the DTA
static volatile uint8_t current_gear = 11;
//! Current engine revs as read by the DTA
static volatile uint16_t current_revs = 1000;
//! Pointer to function call after time elapses.
static void (*volatile end_fun_ptr)(void);

//********* GEAR ***************************************************************

//! Delay between engaging shift cut and running the solenoid. 1 to 2
static const uint16_t SHIFT_CUT_DELAY_1_TO_2 = 300; //50 ms
//! Delay between engaging shift cut and running the solenoid. 2 to 3
static const uint16_t SHIFT_CUT_DELAY_2_TO_3 = 300; //50 ms
//! Delay between engaging shift cut and running the solenoid. 3 to 4
static const uint16_t SHIFT_CUT_DELAY_3_TO_4 = 300; //50 ms
//! Delay between engaging shift cut and running the solenoid. 4 to 5
static const uint16_t SHIFT_CUT_DELAY_4_TO_5 = 300; //50 ms
//! Delay between engaging shift cut and running the solenoid. if unknown gear
static const uint16_t SHIFT_CUT_DELAY_FAIL = 300; //50 ms

//! Time to run the solenoid for gear up.
static const uint16_t GEAR_UP_DELAY_2_TO_5 = 400; //30 ms
//! Time to run the solenoid for gear down.
static const uint16_t GEAR_DOWN_DELAY_5_TO_2 = 400; //30 ms
//! Time to run the solenoid for gear up from first.
static const uint16_t GEAR_UP_DELAY_1_TO_2 = 800; //80 ms
//! Time to run the solenoid for gear down from second.
static const uint16_t GEAR_DOWN_DELAY_2_TO_1 = 800; //80 ms
//! Time to run the solenoid for gear up from neutral.
static const uint16_t GEAR_UP_DELAY_N_TO_2 = 800; //50 ms
//! Time to run the solenoid for gear down from neutral.
static const uint16_t GEAR_DOWN_DELAY_N_TO_1 = 770; //50 ms

//! Lowest revs needed to change up a gear
//static const uint16_t GEAR_DOWN_REV_LIMIT = 9000; // TODO: what should the limit be?

//! Function to start second part of gear up.
static void mid_gear_up(void);
//! Function to end gear up routine.
static void end_gear_change(void);

//********* NEUTRAL ************************************************************

//! Time for the gear pot to stabalise
static const uint16_t NEUTRAL_STABILISATION_DELAY = 10000; // 1 s
//! Number of tries in repeat function.
static const uint8_t NEUTRAL_REPEAT_LIMIT = 10;

//! Last gear selected before neutral attempt.
static volatile uint8_t last_gear = 0;
//! Last delay time used for finding neutral from first.
static volatile uint16_t neutral_1_to_N = 200; // 30 ms
//! Last delay time used for finding neutral from second.
static volatile uint16_t neutral_2_to_N = 250; // 30 ms
//! Number of tries for neutral
static volatile uint8_t neutral_counter = 0;

//***** SINGLE
static void neutral_single_stabiliser_up(void);
static void neutral_single_end_up(void);
static void neutral_single_stabiliser_down(void);
static void neutral_single_end_down(void);

//***** LINEAR
static const uint16_t NEUTRAL_DELAY_ADJUST = 20; //2 ms

static void neutral_repeat_worker_linear(void);
static void neutral_repeat_stabiliser_linear(void);

//***** BISECT
static volatile uint16_t neutral_up_limit_high = 700;
static volatile uint16_t neutral_up_limit_low = 300;
static volatile uint16_t neutral_down_limit_high = 700;
static volatile uint16_t neutral_down_limit_low = 300;

static void neutral_repeat_worker_bisect(void);
static void neutral_repeat_stabiliser_bisect(void);



//********** LAUNCH ************************************************************

// ! Time to run the signal for launch control
//static const uint16_t LAUNCH_SIGNAL_DELAY = 500; //50 ms
//volatile const uint8_t launch_mode = FALSE;
// ! Launch Control, end signal
//static void end_launch_signal(void);

//******************************************************************************
// COMMON
//******************************************************************************

void set_current_gear(uint8_t gear) {
	current_gear = gear;
}

uint8_t get_current_gear(){
	return current_gear;
}

void set_current_revs(uint16_t revs) {
	current_revs = revs;
}

//******************************************************************************
// GEAR CHANGES
//******************************************************************************

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
void gear_up() {
	if (!busy){// && current_gear != 5) {
		busy = TRUE;
		set_output(SHIFT_CUT, GND);
		end_fun_ptr = mid_gear_up;
		switch (current_gear) {
			case 1:
				timer0_start(SHIFT_CUT_DELAY_1_TO_2);
			case 2:
				timer0_start(SHIFT_CUT_DELAY_2_TO_3);
			case 3:
				timer0_start(SHIFT_CUT_DELAY_3_TO_4);
			case 4:
				timer0_start(SHIFT_CUT_DELAY_4_TO_5);
			default:
				timer0_start(SHIFT_CUT_DELAY_1_TO_2);
		}
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
 *
 * If the car is in first gear, trying to shift down will have no effect. However,
 * this is only a feature as long as the DTA is not in fail mode.
 *
 * \param current_gear allows the controler to behave in the most appropriate manner
 */
void gear_down() {
	if (!busy){// && current_gear != 1 ) { // && current_revs < GEAR_DOWN_REV_LIMIT) {
		busy = TRUE;
		set_output(GEAR_DOWN, GND);
		end_fun_ptr = end_gear_change;
		
		if (current_gear == 0) {
			timer0_start(GEAR_DOWN_DELAY_N_TO_1);
		} else if (current_gear == 2) {
			timer0_start(GEAR_DOWN_DELAY_2_TO_1);
		} else {
			timer0_start(GEAR_DOWN_DELAY_5_TO_2);
		}
	}
}
//! Change gear up, part 2
/*!
 * turn off signal for shift cut and engage solenoid, delay for \ref GEAR_UP_DELAY
 * time. end_fun_ptr set to \ref end_gear_change.
 */
static void mid_gear_up(void) {
	set_output(GEAR_UP, GND); // run solenoid
	end_fun_ptr = end_gear_change;
	
	if (current_gear == 0) {
		timer0_start(GEAR_UP_DELAY_N_TO_2);
	} else if (current_gear == 1) {
		timer0_start(GEAR_UP_DELAY_1_TO_2);
	} else {
		timer0_start(GEAR_UP_DELAY_2_TO_5);
	}
}

//! End gear change sequence
/*!
 * Turns off the solenoid in both directions. Frees the busy flag.
 */
static void end_gear_change(void) {
	set_output(SHIFT_CUT, TRI); // reset shift cut output
	set_output(GEAR_UP, TRI); // reset output
	set_output(GEAR_DOWN, TRI); // reset output
	busy = FALSE; // free unit
}

//******************************************************************************
// NEUTRAL
//******************************************************************************

// Single

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
void gear_neutral_single() {
	if (!busy) {
		if (current_gear == 1 || current_gear == POT_FAIL) {
			busy = TRUE;
			set_output(GEAR_UP, GND);
			end_fun_ptr = neutral_single_stabiliser_up;
			timer0_start(neutral_1_to_N);
		} else if (current_gear == 2) {
			busy = TRUE;
			set_output(GEAR_DOWN, GND);
			end_fun_ptr = neutral_single_stabiliser_down;
			timer0_start(neutral_2_to_N);
		}
	}
}

static void neutral_single_stabiliser_up(void) {
	set_output(GEAR_UP, TRI); // reset output
	end_fun_ptr = neutral_single_end_up;
	timer0_start(NEUTRAL_STABILISATION_DELAY);
}

static void neutral_single_end_up(void) {
	if (current_gear == 1) {
		//neutral_1_to_N += NEUTRAL_DELAY_ADJUST;
	} else if (current_gear == 2) {
		//neutral_1_to_N -= NEUTRAL_DELAY_ADJUST;
	}
	busy = FALSE;
}

static void neutral_single_stabiliser_down(void) {
	set_output(GEAR_DOWN, TRI); // reset output
	end_fun_ptr = neutral_single_end_down;
	timer0_start(NEUTRAL_STABILISATION_DELAY);
}

static void neutral_single_end_down(void) {
	if (current_gear == 1) {
		neutral_2_to_N -= NEUTRAL_DELAY_ADJUST;
	} else if (current_gear == 2) {
		neutral_2_to_N += NEUTRAL_DELAY_ADJUST;
	}
	busy = FALSE;
}

//***** LINEAR

void gear_neutral_repeat_linear() {
	if (!busy) {
		last_gear = 0;
		neutral_counter = 0;
		if (current_gear == 1 || current_gear == 2) {
			busy = TRUE;
			neutral_repeat_worker_linear();
			can_setup_tx(0x6000, (uint8_t *) "LIN0", 4);
		} else if (current_gear == 11) {
			gear_neutral_single();
			can_setup_tx(0x6030, (uint8_t *) "SNG0", 4);
		}
	}
}

static void neutral_repeat_worker_linear(void) {
	if (current_gear == 11) {
		gear_neutral_single();
		can_setup_tx(0x6031, (uint8_t *) "SNG1", 4);
		return;
	}

	if (neutral_counter++ == NEUTRAL_REPEAT_LIMIT) {
		busy = FALSE;
		can_setup_tx(0x60ff, (uint8_t *) "STOP", 4);
		return;
	}

	can_setup_tx(0x6011, (uint8_t *) "REPT", 4);

	if (current_gear == 1) {
		//if (last_gear == 0) first attempt
		if (last_gear == 1) { // if last attempt was to soft
			neutral_1_to_N += NEUTRAL_DELAY_ADJUST;
		} else if (last_gear == 2) { // if last attempt was to hard
			neutral_2_to_N -= NEUTRAL_DELAY_ADJUST;
		}
		set_output(GEAR_UP, GND);
		timer0_start(neutral_1_to_N);
	} else if (current_gear == 2) {
		//if (last_gear == 0) first attempt
		if (last_gear == 1) {  // if last attempt was to hard
			neutral_1_to_N -= NEUTRAL_DELAY_ADJUST;
		} else if (last_gear == 2) { // if last attempt was to soft
			neutral_2_to_N += NEUTRAL_DELAY_ADJUST;
		}
		set_output(GEAR_DOWN, GND);
		timer0_start(neutral_2_to_N);
	} else {
		busy = FALSE;
		uint32_t time_info = 0x0000;
		if (last_gear == 1) {
			time_info = (uint32_t) neutral_1_to_N << 16;
		} else if (last_gear == 2) {
			time_info = neutral_2_to_N;
		}
		can_setup_tx(CAN_REAR_LOG_NEUTRAL_ID, (uint8_t *) &time_info, CAN_REAR_LOG_DLC); // send time of successful neutral find
		return;
	}
	last_gear = current_gear;
	end_fun_ptr = neutral_repeat_stabiliser_linear;
}

static void neutral_repeat_stabiliser_linear(void) {
	set_output(GEAR_UP, TRI); // reset output
	set_output(GEAR_DOWN, TRI); // reset output
	end_fun_ptr = neutral_repeat_worker_linear;
	timer0_start(NEUTRAL_STABILISATION_DELAY);
	can_setup_tx(0x6099, (uint8_t *) "LBTS", 4);
}

//***** BISECT

void gear_neutral_repeat_bisect() {
	if (!busy) {
		last_gear = 0;
		neutral_counter = 0;

		neutral_up_limit_high = 700; //70ms
		neutral_up_limit_low  = 300; //30ms
		neutral_down_limit_high = 700; //70ms
		neutral_down_limit_low  = 300; //30ms

		if (current_gear == 1 || current_gear == 2) {
			busy = TRUE;
			neutral_repeat_worker_bisect();
		} else if (current_gear == 11) {
			gear_neutral_single();
		}
	}
}

static void neutral_repeat_worker_bisect(void) {
	if (current_gear == 11) {
		gear_neutral_single();
		return;
	}

	if (neutral_counter++ == NEUTRAL_REPEAT_LIMIT) {
		busy = FALSE;
		return;
	}

	if (current_gear == 1) {
		if (last_gear == 1) { // if last attempt was to soft
			neutral_up_limit_low = neutral_1_to_N;
		} else if (last_gear == 2) { // if last attempt was too hard
			neutral_down_limit_high = neutral_2_to_N;
		}
		neutral_1_to_N = (neutral_up_limit_high + neutral_up_limit_low) >> 1; // div by 2
		set_output(GEAR_UP, GND);
		timer0_start(neutral_1_to_N);
		
	} else if (current_gear == 2) {
		if (last_gear == 1) {  // if last attempt was too hard
			neutral_up_limit_high = neutral_1_to_N;
		} else if (last_gear == 2) { // if last attempt was too soft
			neutral_down_limit_low = neutral_2_to_N;
		}
		neutral_2_to_N = (neutral_down_limit_high + neutral_down_limit_low) >> 1; // div by 2
		set_output(GEAR_DOWN, GND);
		timer0_start(neutral_2_to_N);
		
	} else {
		busy = FALSE; //assume in neutral. (or in third, but that really shouldn't happen.)
		uint32_t time_info = 0x0000;
		if (last_gear == 1) {
			time_info = (uint32_t) neutral_1_to_N << 16;
		} else if (last_gear == 2) {
			time_info = neutral_2_to_N;
		}
		can_setup_tx(CAN_REAR_LOG_NEUTRAL_ID, (uint8_t *) &time_info, CAN_REAR_LOG_DLC); // send time of successful neutral find
		return;
	}
	last_gear = current_gear;
	end_fun_ptr = neutral_repeat_stabiliser_bisect;
}

static void neutral_repeat_stabiliser_bisect(void) {
	set_output(GEAR_UP, TRI); // reset output
	set_output(GEAR_DOWN, TRI); // reset output
	end_fun_ptr = neutral_repeat_worker_bisect;
	timer0_start(NEUTRAL_STABILISATION_DELAY);
}

//******************************************************************************
// LAUNCH
//******************************************************************************

//! Launch Control signal
/*!
 * Sets a signal to the DTA that initiates its launch control system.
 */
/*
 //OSÄKER. ÄNDRA OM HÄR!
void launch_control(void) {}
	if (!busy) {
		busy = TRUE;
		set_output(LAUNCH, GND);
		end_fun_ptr = end_launch_signal;
		timer0_start(LAUNCH_SIGNAL_DELAY);
		if (!launch_mode) {
			clutch_factor = clutch_filter_launch;
			launch_mode = TRUE;
		} else {
			clutch_factor = clutch_filter_main;
			launch_mode = FALSE;
		}
	}
}
*/

/*
//OSÄKER. ÄNDRA OM HÄR!
void launch_stop_clutch(void) {}
	clutch_factor = clutch_filter_main;
	launch_mode = FALSE;
}
*/

//! Launch Control, end signal
/*!
 * stops the signal to the DTA when Launch Control is active.
 */
/*static void end_launch_signal(void) {
	set_output(LAUNCH, TRI);
	busy = FALSE;
}
*/
//******************************************************************************
// TIMER
//******************************************************************************

//! Timed function.
/*!
 * Used for ending gear change routines. Uses the function pointer \ref end_fun_ptr
 * to call the correct next function.
 */
void timer0_isr_stop(void) {
	(*end_fun_ptr)();
}
