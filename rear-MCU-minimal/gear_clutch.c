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

//! Number of cluych readings to average
static const uint16_t CLUTCH_AVERAGE_NUMBER = 10;
//! Symmetry point, the resting value of the clutch position sensor.
static const uint16_t CLUTCH_BIAS_MID = 512;
//! Threshold value for closed clutch
static const uint16_t CLUTCH_POS_CLOSED = 650;
//! Threshold value for open clutch
static const uint16_t CLUTCH_POS_OPEN = 900;
//! PWM value for closed clutch
static const uint16_t CLUTCH_DC_CLOSED = 3000;
//! PWM value for open clutch
static const uint16_t CLUTCH_DC_OPEN = 13000;

//! Array holding the \ref CLUTCH_AVERAGE_NUMBER number of readings to average.
volatile static uint16_t clutch_array[CLUTCH_AVERAGE_NUMBER];
volatile static uint8_t clutch_array_counter = 0;

//! Pointer to function call after time elapses.
static void (*volatile end_fun_ptr)(void);
//! Function to start second part of gear up.
static void mid_gear_up(void);
//! Function to end gear up routine.
static void end_gear_change(void);
//! Launch Control, end signal
static void end_launch_signal(void);

void gear_up(uint8_t current_gear) {
	if (!busy && current_gear != 5) {
		busy = TRUE;
		set_output(SHIFT_CUT, GND);
		end_fun_ptr = mid_gear_up;
		timer0_start(SHIFT_CUT_DELAY);
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

void gear_down(uint8_t current_gear) {
	if (!busy && current_gear != 1) {
		busy = TRUE;
		set_output(GEAR_DOWN, GND);
		end_fun_ptr = end_gear_change;
		timer0_start(GEAR_DOWN_DELAY);
	}
}

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

void clutch_set(uint16_t pos) {
	
	clutch_array[clutch_array_counter++] = pos;
	clutch_array_counter %= CLUTCH_AVERAGE_NUMBER;
	
	float clutch_ = 0;
	for (uint8_t i = 0; i<CLUTCH_AVERAGE_NUMBER; i++) {
		clutch_average += clutch_array[i];
	}
	clutch_average /= CLUTCH_AVERAGE_NUMBER;
	
	if (clutch_average < CLUTCH_BIAS_MID) {
		clutch_average = 1024 - clutch_average;
	}
	if (clutch_average < CLUTCH_POS_CLOSED) {
		timer1_dutycycle(CLUTCH_DC_CLOSED);
	} else if (clutch_average > CLUTCH_POS_OPEN) {
		timer1_dutycycle(CLUTCH_DC_OPEN);
	} else {
		timer1_dutycycle((uint16_t) ((clutch_average - CLUTCH_POS_CLOSED) * ((float) (CLUTCH_DC_OPEN - CLUTCH_DC_CLOSED)) / (CLUTCH_POS_OPEN - CLUTCH_POS_CLOSED) + CLUTCH_DC_CLOSED));
	}
}

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

void timer0_isr_stop(void) {
	(*end_fun_ptr)();
}
