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

static const float CLUTCH_POS_RIGHT_CLOSED = 360;
static const float CLUTCH_POS_RIGHT_OPEN = 485;
static const float CLUTCH_DC_CLOSED = 4000;
static const float CLUTCH_DC_OPEN = 12000;

static const float clutch_pos_right_break_closed = 410;
static const float clutch_pos_right_break_open   = 460;

static const float clutch_dc_break_closed = 7000;
static const float clutch_dc_break_open   = 9000;

volatile static float clutch_right_old = 0;
static const float clutch_factor = 0.1;

volatile static float clutch_right_factor_open   = 0;
volatile static float clutch_right_factor_mid    = 0;
volatile static float clutch_right_factor_closed = 0;

void clutch_init(void) {
	clutch_right_factor_closed = ((clutch_dc_break_closed - CLUTCH_DC_CLOSED))       / (clutch_pos_right_break_closed - CLUTCH_POS_RIGHT_CLOSED);
	clutch_right_factor_mid    = ((clutch_dc_break_open   - clutch_dc_break_closed)) / (clutch_pos_right_break_open   - clutch_pos_right_break_closed);
	clutch_right_factor_open   = ((CLUTCH_DC_OPEN         - clutch_dc_break_open))   / (CLUTCH_POS_RIGHT_OPEN         - clutch_pos_right_break_open);
}

void clutch_set(uint16_t pos_left, uint16_t pos_right) {
	float clutch_right_new = clutch_factor * pos_right + (1 - clutch_factor) * clutch_right_old;
	clutch_right_old = clutch_right_new;

	float duty_right = CLUTCH_DC_CLOSED;

	if (clutch_right_new > CLUTCH_POS_RIGHT_OPEN) {
		timer1_dutycycle(CLUTCH_DC_OPEN);
		return;
	} else if (clutch_right_new > clutch_pos_right_break_open) {
		duty_right = ((clutch_right_new - clutch_pos_right_break_open) * clutch_right_factor_open + clutch_dc_break_open);
	} else if (clutch_right_new > clutch_pos_right_break_closed) {
		duty_right = ((clutch_right_new - clutch_pos_right_break_closed) * clutch_right_factor_mid + clutch_dc_break_closed);
	} else if (clutch_right_new > CLUTCH_POS_RIGHT_CLOSED) {
		duty_right = ((clutch_right_new - CLUTCH_POS_RIGHT_CLOSED) * clutch_right_factor_closed + CLUTCH_DC_CLOSED);
	}

	timer1_dutycycle(duty_right);
}
