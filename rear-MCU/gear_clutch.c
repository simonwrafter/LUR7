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

static volatile uint8_t busy = FALSE;
static volatile uint8_t shift_cut_flag = FALSE;
static volatile uint8_t gear_up_flag = FALSE;
static volatile uint8_t gear_down_flag = FALSE;
static volatile uint8_t neutral_up_flag = FALSE;
static volatile uint8_t neutral_down_flag = FALSE;

static const uint16_t SHIFT_CUT_DELAY = 1000;
static const uint16_t GEAR_UP_DELAY = 1000;
static const uint16_t GEAR_DOWN_DELAY = 1000;
static const uint16_t NEUTRAL_UP_DELAY = 1000;
static const uint16_t NEUTRAL_DOWN_DELAY = 1000;

//! H
/*
 * b
 */
void gear_up(void) {
	if (!busy) {
		busy = TRUE;
		shift_cut_flag = TRUE;
		gear_up_flag = TRUE;
		set_output(SHIFT_CUT, GND);
		timer0_start(SHIFT_CUT_DELAY);
	}
}

//! H
/*
 * b
 */
void gear_down(void) {
	if (!busy) {
		busy = TRUE;
		gear_down_flag = TRUE;
		set_output(GEAR_DOWN, GND);
		timer0_start(GEAR_DOWN_DELAY);
	}
}

//! H
/*
 * b
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

//! H
/*
 * b
 */
void clutch_set(uint16_t pos) {
	
}


/*!
 * Used for ending gear change routine.
 */
void timer0_isr_stop(void) {
	if (shift_cut_flag) {
		shift_cut_flag = FALSE;
		set_output(SHIFT_CUT, TRI);
		set_output(GEAR_UP, GND);
		timer0_start(GEAR_UP_DELAY);
	} else if (gear_up_flag) {
		gear_up_flag = FALSE;
		set_output(GEAR_UP, TRI);
		busy = FALSE;
	} else if (gear_down_flag) {
		gear_down_flag = FALSE;
		set_output(GEAR_DOWN, TRI);
		busy = FALSE;
	} else if (neutral_up_flag) {
		neutral_up_flag = FALSE;
		set_output(GEAR_UP, TRI);
		busy = FALSE;
	} else if (neutral_down_flag) {
		neutral_down_flag = FALSE;
		set_output(GEAR_DOWN, TRI);
		busy = FALSE;
	}
}
