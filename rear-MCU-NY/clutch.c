/*
 * clutch.c - A collection of functions to setup and ease the use of the LUR7 PCB
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
#include "config.h"
#include "clutch.h"

//! Threshold value for closed clutch
static const float CLUTCH_POS_LEFT_LOOSE  = 553; // slapp vajer
//! Threshold value for open clutch
static const float CLUTCH_POS_LEFT_TIGHT  = 639; // dragen vajer
//! Threshold value for closed clutch
static const float CLUTCH_POS_RIGHT_LOOSE = 427;
//! Threshold value for open clutch
static const float CLUTCH_POS_RIGHT_TIGHT = 378;
//! PWM value for closed clutch
static const float CLUTCH_DC_LOOSE        = 6000; // slapp vajer. min: 2200
//! PWM value for open clutch
static const float CLUTCH_DC_TIGHT        = 13500; // dragen vajer. max: 13500 (?)

static const float clutch_pos_break_factor   = 0.8; // AJUST IF NEEDED!
volatile static float clutch_pos_left_break  = 625;
volatile static float clutch_pos_right_break = 387;

static const float clutch_dc_break = 9500; // AJUST IF NEEDED!

//! Initial value for the filter.
volatile static float clutch_left_filtered  = 1000;
//! Initial value for the filter.
volatile static float clutch_right_filtered = 0;

volatile static float duty_left  = 0;
volatile static float duty_right = 0;

//! The filter factor for the new clutch position value.
static const float clutch_factor = 0.1;

volatile static float clutch_left_factor_open    = 0;
volatile static float clutch_left_factor_closed  = 0;
volatile static float clutch_right_factor_open   = 0;
volatile static float clutch_right_factor_closed = 0;


void clutch_init(void) {
	timer1_dutycycle(CLUTCH_DC_LOOSE);
	clutch_left_factor_closed  = ((clutch_dc_break - CLUTCH_DC_LOOSE))  / (clutch_pos_left_break - CLUTCH_POS_LEFT_LOOSE);
	clutch_left_factor_open    = ((CLUTCH_DC_TIGHT  - clutch_dc_break))   / (CLUTCH_POS_LEFT_TIGHT  - clutch_pos_left_break);

	clutch_right_factor_closed = ((clutch_dc_break - CLUTCH_DC_LOOSE)) / (clutch_pos_right_break - CLUTCH_POS_RIGHT_LOOSE);
	clutch_right_factor_open   = ((CLUTCH_DC_TIGHT  - clutch_dc_break))  / (CLUTCH_POS_RIGHT_TIGHT  - clutch_pos_right_break);
	
	//clutch_pos_left_break      = CLUTCH_POS_LEFT_LOOSE + (CLUTCH_POS_LEFT_TIGHT - CLUTCH_POS_LEFT_LOOSE) * clutch_pos_break_factor;
	//clutch_pos_right_break     = CLUTCH_POS_RIGHT_TIGHT + (CLUTCH_POS_RIGHT_LOOSE - CLUTCH_POS_RIGHT_TIGHT) * (1.0 - clutch_pos_break_factor);
}

void clutch_filter_left(uint16_t pos_left) {
	uint16_t clf = clutch_factor * pos_left + (1 - clutch_factor) * clutch_left_filtered;
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		clutch_left_filtered = clf;
	} // end ATOMIC_BLOCK
}

void clutch_filter_right(uint16_t pos_right) {
	float crf = clutch_factor * pos_right + (1 - clutch_factor) * clutch_right_filtered;
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		clutch_right_filtered = crf;
	} // end ATOMIC_BLOCK
}

void clutch_dutycycle_left(void) {
	if (clutch_left_filtered > CLUTCH_POS_LEFT_TIGHT) {
		duty_left = CLUTCH_DC_TIGHT;
	} else if (clutch_left_filtered > clutch_pos_left_break) {
		duty_left = ((clutch_left_filtered - clutch_pos_left_break)  * clutch_left_factor_open   + clutch_dc_break);
	} else if (clutch_left_filtered > CLUTCH_POS_LEFT_LOOSE) {
		duty_left = ((clutch_left_filtered - CLUTCH_POS_LEFT_LOOSE) * clutch_left_factor_closed + CLUTCH_DC_LOOSE);
	} else {
		duty_left = CLUTCH_DC_LOOSE;
	}
}

void clutch_dutycycle_right(void) {
	if (clutch_right_filtered < CLUTCH_POS_RIGHT_TIGHT) {
		duty_right = CLUTCH_DC_TIGHT;
	} else if (clutch_right_filtered < clutch_pos_right_break) {
		duty_right = ((clutch_right_filtered - clutch_pos_right_break)  * clutch_right_factor_open   + clutch_dc_break);
	} else if (clutch_right_filtered < CLUTCH_POS_RIGHT_LOOSE) {
		duty_right = ((clutch_right_filtered - CLUTCH_POS_RIGHT_LOOSE) * clutch_right_factor_closed + CLUTCH_DC_LOOSE);
	} else {
		duty_right = CLUTCH_DC_LOOSE;
	}
}

void clutch_set_dutycycle(void) {
	if (duty_left > duty_right) {
		timer1_dutycycle(duty_left);
	} else {
		timer1_dutycycle(duty_right);
	}
}

uint16_t clutch_get_filtered_left(void) {
	return (uint16_t) clutch_left_filtered;
}

uint16_t clutch_get_filtered_right(void) {
	return (uint16_t) clutch_right_filtered;
}

uint16_t clutch_get_dutycycle_left(void) {
	return (uint16_t) duty_left;
}

uint16_t clutch_get_dutycycle_right(void) {
	return (uint16_t) duty_right;
}


/* //single linear
if (clutch_right_new < CLUTCH_POS_RIGHT_LOOSE) {
	timer1_dutycycle(CLUTCH_DC_LOOSE);
} else if (clutch_right_new > CLUTCH_POS_RIGHT_TIGHT) {
	timer1_dutycycle(CLUTCH_DC_TIGHT);
} else {
	timer1_dutycycle((clutch_right_new - CLUTCH_POS_RIGHT_LOOSE) * (CLUTCH_DC_OPEN - CLUTCH_DC_LOOSE) / (CLUTCH_POS_RIGHT_OPEN - CLUTCH_POS_RIGHT_LOOSE) + CLUTCH_DC_LOOSE);
} */
