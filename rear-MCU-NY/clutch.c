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
static const float CLUTCH_POS_LEFT_CLOSED  = 360; // slapp vajer
//! Threshold value for open clutch
static const float CLUTCH_POS_LEFT_OPEN    = 485; // dragen vajer
//! Threshold value for closed clutch
static const float CLUTCH_POS_RIGHT_CLOSED = 360; //DONE
//! Threshold value for open clutch
static const float CLUTCH_POS_RIGHT_OPEN   = 485; //DONE
//! PWM value for closed clutch
static const float CLUTCH_DC_CLOSED        = 13000; //FINE TUNE // slapp vajer. max: 13700
//! PWM value for open clutch
static const float CLUTCH_DC_OPEN          = 4500; //FINE TUNE // dragen vajer. min: 2200 (?)

static const float clutch_pos_left_break   = 460;
static const float clutch_pos_right_break  = 460;

static const float clutch_dc_break   = 9500; //trim!!!

//! Initial value for the filter.
volatile static float clutch_left_filtered = 0;
//! Initial value for the filter.
volatile static float clutch_right_filtered = 0;

volatile static float duty_left = 0;
volatile static float duty_right = 0;

//! The filter factor for the new clutch position value.
static const float clutch_factor = 0.1;

volatile static float clutch_left_factor_open    = 0;
volatile static float clutch_left_factor_closed  = 0;
volatile static float clutch_right_factor_open   = 0;
volatile static float clutch_right_factor_closed = 0;


void clutch_init(void) {
	clutch_left_factor_closed  = ((clutch_dc_break - CLUTCH_DC_CLOSED))  / (clutch_pos_left_break - CLUTCH_POS_LEFT_CLOSED);
	clutch_left_factor_open    = ((CLUTCH_DC_OPEN  - clutch_dc_break))   / (CLUTCH_POS_LEFT_OPEN  - clutch_pos_left_break);

	clutch_right_factor_closed = ((clutch_dc_break - CLUTCH_DC_CLOSED)) / (clutch_pos_right_break - CLUTCH_POS_RIGHT_CLOSED);
	clutch_right_factor_open   = ((CLUTCH_DC_OPEN  - clutch_dc_break))  / (CLUTCH_POS_RIGHT_OPEN  - clutch_pos_right_break);
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
	if (clutch_left_filtered > CLUTCH_POS_LEFT_OPEN) {
		duty_left = CLUTCH_DC_OPEN;
	} else if (clutch_left_filtered > clutch_pos_left_break) {
		duty_left = ((clutch_left_filtered - clutch_pos_left_break)  * clutch_left_factor_open   + clutch_dc_break);
	} else if (clutch_left_filtered > CLUTCH_POS_LEFT_CLOSED) {
		duty_left = ((clutch_left_filtered - CLUTCH_POS_LEFT_CLOSED) * clutch_left_factor_closed + CLUTCH_DC_CLOSED);
	} else {
		duty_left = CLUTCH_DC_CLOSED;
	}
}

void clutch_dutycycle_right(void) {
	if (clutch_right_filtered > CLUTCH_POS_RIGHT_OPEN) {
		duty_right = CLUTCH_DC_OPEN;
	} else if (clutch_right_filtered > clutch_pos_right_break) {
		duty_right = ((clutch_right_filtered - clutch_pos_right_break)  * clutch_right_factor_open   + clutch_dc_break);
	} else if (clutch_right_filtered > CLUTCH_POS_RIGHT_CLOSED) {
		duty_right = ((clutch_right_filtered - CLUTCH_POS_RIGHT_CLOSED) * clutch_right_factor_closed + CLUTCH_DC_CLOSED);
	} else {
		duty_left = CLUTCH_DC_CLOSED;
	}
}

void clutch_set_dutycycle(void) {
	if (duty_left < duty_right) {
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
if (clutch_right_new < CLUTCH_POS_RIGHT_CLOSED) {
	timer1_dutycycle(CLUTCH_DC_CLOSED);
} else if (clutch_right_new > CLUTCH_POS_RIGHT_OPEN) {
	timer1_dutycycle(CLUTCH_DC_OPEN);
} else {
	timer1_dutycycle((clutch_right_new - CLUTCH_POS_RIGHT_CLOSED) * (CLUTCH_DC_OPEN - CLUTCH_DC_CLOSED) / (CLUTCH_POS_RIGHT_OPEN - CLUTCH_POS_RIGHT_CLOSED) + CLUTCH_DC_CLOSED);
} */
