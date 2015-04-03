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

#include "../header_and_config/LUR7.h"
#include "gear_clutch_launch.h"
#include "config.h"


//********** COMMON ************************************************************

//! Set to TRUE when system is working.
static volatile uint8_t busy = FALSE;
//! Currently selected gear as read by the DTA
static volatile uint8_t current_gear = 11;
//! Pointer to function call after time elapses.
static void (*volatile end_fun_ptr)(void);

//********* GEAR ***************************************************************

//! Delay between engaging shift cut and running the solenoid.
static const uint16_t SHIFT_CUT_DELAY = 1000; //100 ms
//! Time to run the solenoid for gear up.
static const uint16_t GEAR_UP_DELAY = 1000; //100 ms
//! Time to run the solenoid for gear down.
static const uint16_t GEAR_DOWN_DELAY = 1000; //100 ms

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
static volatile uint16_t neutral_up_try_time = 500;
//! Last delay time used for finding neutral from second.
static volatile uint16_t neutral_down_try_time = 500;
//! Number of tries for neutral
static volatile uint8_t neutral_counter = 0;

//***** SINGLE
static void neutral_single_stabiliser_up(void);
static void neutral_single_end_up(void);
static void neutral_single_stabiliser_down(void);
static void neutral_single_end_down(void);

//***** LINEAR
static const uint16_t NEUTRAL_DELAY_ADJUST = 50; //5 ms

static void neutral_repeat_worker_linear(void);
static void neutral_repeat_stabiliser_linear(void);

//***** BINARY
static volatile uint16_t neutral_up_limit_high = 700;
static volatile uint16_t neutral_up_limit_low = 300;
static volatile uint16_t neutral_down_limit_high = 700;
static volatile uint16_t neutral_down_limit_low = 300;

static void neutral_repeat_worker_binary(void);
static void neutral_repeat_stabiliser_binary(void);

//********** CLUTCH ************************************************************

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

//! Initial value for the filter.
volatile static float clutch_old = 512;
//! The filter factor for the new clutch position value.
static const float clutch_factor = 0.1;

//********** LAUNCH ************************************************************

//! Time to run the signal for launch control
static const uint16_t LAUNCH_SIGNAL_DELAY = 500; //50 ms
//! Launch Control, end signal
static void end_launch_signal(void);

//******************************************************************************
// COMMON
//******************************************************************************

void set_current_gear(uint8_t gear) {
	current_gear = gear;
}

//******************************************************************************
// GEAR CHANGES
//******************************************************************************

void gear_up() {
	if (!busy && current_gear != 5) {
		busy = TRUE;
		set_output(SHIFT_CUT, GND);
		end_fun_ptr = mid_gear_up;
		timer0_start(SHIFT_CUT_DELAY);
	}
}

void gear_down() {
	if (!busy && current_gear != 1) {
		busy = TRUE;
		set_output(GEAR_DOWN, GND);
		end_fun_ptr = end_gear_change;
		timer0_start(GEAR_DOWN_DELAY);
	}
}

static void mid_gear_up(void) {
	set_output(SHIFT_CUT, TRI); // reset shift cut output
	set_output(GEAR_UP, GND); // run solenoid
	end_fun_ptr = end_gear_change;
	timer0_start(GEAR_UP_DELAY); // new delay for actual gear change
}

static void end_gear_change(void) {
	set_output(GEAR_UP, TRI); // reset output
	set_output(GEAR_DOWN, TRI); // reset output
	busy = FALSE; // free unit
}

//******************************************************************************
// NEUTRAL
//******************************************************************************

void gear_neutral_single() {
	if (!busy) {
		if (current_gear == 1 || current_gear == 11) {
			busy = TRUE;
			set_output(GEAR_UP, GND);
			end_fun_ptr = neutral_single_stabiliser_up;
			timer0_start(neutral_up_try_time);
		} else if (current_gear == 2) {
			busy = TRUE;
			set_output(GEAR_DOWN, GND);
			end_fun_ptr = neutral_single_stabiliser_down;
			timer0_start(neutral_down_try_time);
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
		neutral_up_try_time += NEUTRAL_DELAY_ADJUST;
	} else if (current_gear == 2) {
		neutral_up_try_time -= NEUTRAL_DELAY_ADJUST;
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
		neutral_down_try_time -= NEUTRAL_DELAY_ADJUST;
	} else if (current_gear == 2) {
		neutral_down_try_time += NEUTRAL_DELAY_ADJUST;
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
		} else if (current_gear == 11) {
			gear_neutral_single();
		}
	}
}

static void neutral_repeat_worker_linear(void) {
	if (current_gear == 11) {
		gear_neutral_single();
		return;
	}

	if (neutral_counter++ == NEUTRAL_REPEAT_LIMIT) {
		busy = FALSE;
		return;
	}

	if (current_gear == 1 && last_gear == 0) { //first attempt, starting in first
		set_output(GEAR_UP, GND);
		timer0_start(neutral_up_try_time);
	} else if (current_gear == 1 && last_gear == 1) { // if last attempt was to soft
		set_output(GEAR_UP, GND);
		timer0_start(neutral_up_try_time += NEUTRAL_DELAY_ADJUST);
	} else if (current_gear == 1 && last_gear == 2) { // if last attempt was to hard
		set_output(GEAR_UP, GND);
		neutral_down_try_time -= NEUTRAL_DELAY_ADJUST;
		timer0_start(neutral_up_try_time);
	} else if (current_gear == 2 && last_gear == 0) { //first attempt, starting in second
		set_output(GEAR_DOWN, GND);
		timer0_start(neutral_down_try_time);
	} else if (current_gear == 2 && last_gear == 1) {  // if last attempt was to hard
		set_output(GEAR_DOWN, GND);
		neutral_up_try_time -= NEUTRAL_DELAY_ADJUST;
		timer0_start(neutral_down_try_time);
	} else if (current_gear == 2 && last_gear == 2) { // if last attempt was to soft
		set_output(GEAR_DOWN, GND);
		timer0_start(neutral_down_try_time += NEUTRAL_DELAY_ADJUST);
	} else {
		busy = FALSE;
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
}

//***** BINARY

void gear_neutral_repeat_binary() {
	if (!busy) {
		last_gear = 0;
		neutral_counter = 0;

		//** binary *************
		neutral_up_limit_high = 700;
		neutral_up_limit_low  = 300;
		neutral_down_limit_high = 700;
		neutral_down_limit_low  = 300;
		//***********************

		if (current_gear == 1 || current_gear == 2) {
			busy = TRUE;
			neutral_repeat_worker_binary();
		} else if (current_gear == 11) {
			gear_neutral_single();
		}
	}
}

static void neutral_repeat_worker_binary(void) {
	if (current_gear == 11) {
		gear_neutral_single();
		return;
	}

	if (neutral_counter++ == NEUTRAL_REPEAT_LIMIT) {
		busy = FALSE;
		return;
	}

	// binary
	if (current_gear == 1) {
		if (last_gear == 1) { // if last attempt was to soft
			neutral_up_limit_low = neutral_up_try_time;
		} else if (last_gear == 2) { // if last attempt was to hard
			neutral_down_limit_high = neutral_down_try_time;
		}
		neutral_up_try_time = (neutral_up_limit_high + neutral_up_limit_low) << 1; // div by 2
		set_output(GEAR_UP, GND);
		timer0_start(neutral_up_try_time);
	} else if (current_gear == 2) {
		if (last_gear == 1) {  // if last attempt was to hard
			neutral_up_limit_high = neutral_up_try_time;
		} else if (last_gear == 2) { // if last attempt was to soft
			neutral_down_limit_low = neutral_down_try_time;
		}
		set_output(GEAR_DOWN, GND);
		timer0_start(neutral_down_try_time);
	} else {
		busy = FALSE; //assume in neutral. (or in third, but that really shouldn't happen.)
		return;
	}

	last_gear = current_gear;
	end_fun_ptr = neutral_repeat_stabiliser_binary;
}

static void neutral_repeat_stabiliser_binary(void) {
	set_output(GEAR_UP, TRI); // reset output
	set_output(GEAR_DOWN, TRI); // reset output
	end_fun_ptr = neutral_repeat_worker_binary;
	timer0_start(NEUTRAL_STABILISATION_DELAY);
}

//******************************************************************************
// CLUTCH
//******************************************************************************

void clutch_set(uint16_t pos) {
	float clutch_new = clutch_factor * pos + (1 - clutch_factor) * clutch_old;
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

//******************************************************************************
// LAUNCH
//******************************************************************************

void launch_control(void) {
	if (!busy) {
		busy = TRUE;
		set_output(LAUNCH, GND);
		end_fun_ptr = end_launch_signal;
		timer0_start(LAUNCH_SIGNAL_DELAY);
	}
}

static void end_launch_signal(void) {
	set_output(LAUNCH, TRI);
	busy = FALSE;
}

//******************************************************************************
// TIMER
//******************************************************************************

void timer0_isr_stop(void) {
	(*end_fun_ptr)();
}
