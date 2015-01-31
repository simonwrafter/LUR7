/*
 * / midMCU.c - A collection of functions to setup and ease the use of the LUR7 PCB
 * / Copyright (C) 2014  Simon Wrafter <simon.wrafter@gmail.com>
 * /
 * / This program is free software: you can redistribute it and/or modify
 * / it under the terms of the GNU General Public License as published by
 * / the Free Software Foundation, either version 3 of the License, or
 * / (at your option) any later version.
 * /
 * / This program is distributed in the hope that it will be useful,
 * / but WITHOUT ANY WARRANTY; without even the implied warranty of
 * / MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * / GNU General Public License for more details.
 * /
 * / You should have received a copy of the GNU General Public License
 * / along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../header_and_config/LUR7.h"
#include "midMCU.h"

static volatile uint16_t revs = 0;
static volatile uint16_t gear = 0;
static volatile uint16_t speed = 0;
static volatile uint16_t log_id = 0;
static volatile uint16_t temperature = 0;

uint8_t convert_revs_to_bar() {
	uint8_t return_val = (revs - REV_MIN) / (REV_MAX - REV_MIN) * BAR_HIGH + BAR_LOW;
	if (return_val < BAR_LOW) {
		return 0;
	} else if (return_val > BAR_HIGH) {
		return BAR_HIGH;
	}
	return return_val;
}

uint8_t bin_to_7seg(uint8_t binary, uint8_t dp) {
	if (binary >= 0 && binary <= 10) {
		if (!dp) {
			return sev_seg[binary];
		} else {
			return sev_seg[binary] + 1;
		}
	}
	return sev_seg[10];
}

void pcISR_in1(void) {}
void pcISR_in2(void) {}
void pcISR_in3(void) {}
void pcISR_in4(void) {}
void pcISR_in5(void) {}
void pcISR_in6(void) {}
void pcISR_in7(void) {}
void pcISR_in8(void) {}
void pcISR_in9(void) {}