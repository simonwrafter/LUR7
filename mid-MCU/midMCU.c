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
#include "shiftregister.h"

static volatile uint16_t revs = 0;
static volatile uint8_t  gear = 0;
static volatile uint16_t speed = 0;
static volatile uint16_t log_id = 0;
static volatile uint16_t temperature = 0;
static volatile uint8_t  bcd_vect[3] = {0,0,0};

uint8_t revs_to_bar() {
	uint8_t return_val = (revs - REV_MIN) / (REV_MAX - REV_MIN) * REV_BAR_MAX + REV_BAR_MIN;
	if (return_val < REV_BAR_MIN) {
		return 0;
	} else if (return_val > REV_BAR_MAX) {
		return REV_BAR_MAX;
	}
	return return_val;
}

uint8_t temp_to_bar() {
	if (temperature < TEMP_LVL_1) {
		return 0;
	} else if (temperature < TEMP_LVL_2) {
		return 1;
	} else if (temperature < TEMP_LVL_3) {
		return 2;
	} else if (temperature < TEMP_LVL_4) {
		return 3;
	} else if (temperature < TEMP_LVL_5) {
		return 4;
	} else if (temperature < TEMP_LVL_6) {
		return 5;
	} else if (temperature < TEMP_LVL_7) {
		return 6;
	} else if (temperature < TEMP_LVL_8) {
		return 7;
	} else if (temperature < TEMP_LVL_9) {
		return 8;
	} else if (temperature < TEMP_LVL_10) {
		return 9;
	} else {
		return 10;
	}
}


void bcd_speed(void) {
	uint16_t holder = speed;

	if (holder >= 100) {
		bcd_vect[0] = 0;
		while (holder >= 100) {
			holder -= 100;
			bcd_vect[0]++;
		}
	} else {
		bcd_vect[0] = 10;
	}

	if (holder >= 10) {
		bcd_vect[1] = 0;
		while (holder >= 10) {
			holder -= 10;
			bcd_vect[1]++;
		}
	} else {
		bcd_vect[1] = 10;
	}

	bcd_vect[2] = 0;
	while (holder >= 1) {
		holder -= 1;
		bcd_vect[2]++;
	}

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

void update_display(void) {
	shift_bar(temp_to_bar(), 10);
	shift_bar(revs_to_bar(), 22);

	bcd_speed();
	shift_byte(bin_to_7seg(bcd_vect[0], OFF));
	shift_byte(bin_to_7seg(bcd_vect[1], OFF));
	shift_byte(bin_to_7seg(bcd_vect[2], OFF));

	shift_byte(bin_to_7seg(gear, OFF));

	shift_strobe();
}
