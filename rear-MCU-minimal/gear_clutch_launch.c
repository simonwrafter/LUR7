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

//! Set to TRUE when system is working.
static volatile uint8_t busy = FALSE;

//! Currently selected gear as read by the DTA
static volatile uint8_t current_gear = 11;

//! Delay between engaging shift cut and running the solenoid.
static const uint16_t SHIFT_CUT_DELAY = 1000; //100 ms
//! Time to run the solenoid for gear up.
static const uint16_t GEAR_UP_DELAY = 1000; //100 ms
//! Time to run the solenoid for gear down.
static const uint16_t GEAR_DOWN_DELAY = 1000; //100 ms
//! Time to add to or subtract from neutral delay time when searching
static const uint16_t NEUTRAL_DELAY_ADJUST = 50; //5 ms
//! Time for the gear pot to stabalise
static const uint16_t NEUTRAL_STABILISATION_DELAY = 10000; // 1 s

//! Time to run the signal for launch control
static const uint16_t LAUNCH_SIGNAL_DELAY = 500; //50 ms

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

//! Last gear selected befor neutral attempt.
static volatile uint8_t last_gear = 0;
//! Last delay time used for finding neutral from first.
static volatile uint16_t last_up_time = 500;
//! Last delay time used for finding neutral from second.
static volatile uint16_t last_down_time = 500;
//! Number of tries for neutral
static volatile uint8_t neutral_counter = 0;

//! Initial value for the filter.
volatile static float clutch_old = 512;
//! The filter factor for the new clutch position value.
static const float clutch_factor = 0.1;

//! Pointer to function call after time elapses.
static void (*volatile end_fun_ptr)(void);

//! Function to start second part of gear up.
static void mid_gear_up(void);
//! Function to end gear up routine.
static void end_gear_change(void);
//! Launch Control, end signal
static void end_launch_signal(void);

static void neutral_repeat_worker(void);
static void neutral_repeat_stabiliser(void);

//******************************************************************************
// GEAR CHANGES
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
			set_output(GEAR_UP, GND);
			end_fun_ptr = end_gear_change;
			timer0_start(last_up_time);
		} else if (current_gear == 2) {
			busy = TRUE;
			set_output(GEAR_DOWN, GND);
			end_fun_ptr = end_gear_change;
			timer0_start(last_down_time);
		}
	}
}

void gear_neutral_repeat() {
	if (!busy) {
		last_gear = 0;
		neutral_counter = 0;
		if (current_gear == 1 || current_gear == 2) {
			busy = TRUE;
			neutral_repeat_worker();
		}
	}
}

static void neutral_repeat_worker(void) {
	if (current_gear == 11) {
		gear_neutral_single();
		return;
	}

	if (neutral_counter++ == 10) {
		busy = FALSE;
		return;
	}

	if (current_gear == 1 && last_gear == 0) { //first attempt, starting in first
		set_output(GEAR_UP, GND);
		timer0_start(last_up_time);
	} else if (current_gear == 1 && last_gear == 1) { // if last attempt was to soft
		set_output(GEAR_UP, GND);
		timer0_start(last_up_time += NEUTRAL_DELAY_ADJUST);
	} else if (current_gear == 1 && last_gear == 2) { // if last attempt was to hard
		set_output(GEAR_UP, GND);
		last_down_time -= NEUTRAL_DELAY_ADJUST;
		timer0_start(last_up_time);
	} else if (current_gear == 2 && last_gear == 0) { //first attempt, starting in second
		set_output(GEAR_DOWN, GND);
		timer0_start(last_down_time);
	} else if (current_gear == 2 && last_gear == 1) {  // if last attempt was to hard
		set_output(GEAR_DOWN, GND);
		last_up_time -= NEUTRAL_DELAY_ADJUST;
		timer0_start(last_down_time);
	} else if (current_gear == 2 && last_gear == 2) { // if last attempt was to soft
		set_output(GEAR_DOWN, GND);
		timer0_start(last_down_time += NEUTRAL_DELAY_ADJUST);
	} else {
		busy = FALSE;
		return;
	}
	last_gear = current_gear;
	end_fun_ptr = neutral_repeat_stabiliser;
}

static void neutral_repeat_stabiliser(void) {
	set_output(GEAR_UP, TRI); // reset output
	set_output(GEAR_DOWN, TRI); // reset output
	end_fun_ptr = neutral_repeat_worker;
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
