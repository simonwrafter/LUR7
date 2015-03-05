/*
 * midMCU.c - A collection of functions to setup and ease the use of the LUR7 PCB
 * Copyright (C) 2014  Simon Wrafter <simon.wrafter@gmail.com>
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
#include "display.h"
#include "shiftregister.h"

static const uint8_t sev_seg[11] = {
	// a b c d e f g dp
	0b11111100, //0
	0b01100000, //1
	0b11011010, //2
	0b11110010, //3
	0b01100110, //4
	0b10110110, //5
	0b10111110, //6
	0b11100000, //7
	0b11111110, //8
	0b11110110, //9
	0b00000000  //blank
};

static volatile uint16_t revs = 8000;
static volatile uint8_t  gear = 0;
static volatile uint16_t speed = 88;
static volatile uint16_t log_id = 0;
static volatile uint16_t water_temp = 20;
static volatile uint16_t oil_temp = 30;
static volatile uint8_t  bcd_vect[3] = {0,0,0};

void update_RPM(uint16_t new_RPM) {
	revs = RPM;
}

void update_gear(uint8_t new_gear) {
	gear = new_gear;
}

void update_speed(uint16_t new_speed) {
	speed = new_speed;
}

void update_logid(uint16_t new_logid) {
	log_id = new_logid;
}

void update_watertemp(uint16_t new_watertemp) {
	water_temp = new_watertemp;
}

void update_oiltemp(uint16_t new_oiltemp) {
	oil_temp = new_oiltemp;
}

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

void bcd_convert(uint16_t value) {
	if (value >= 100) {
		bcd_vect[0] = 0;
		while (value >= 100) {
			value -= 100;
			bcd_vect[0]++;
		}
	} else {
		bcd_vect[0] = 10;
	}

	if (value >= 10) {
		bcd_vect[1] = 0;
		while (value >= 10) {
			value -= 10;
			bcd_vect[1]++;
		}
	} else {
		bcd_vect[1] = 10;
	}

	bcd_vect[2] = 0;
	while (value >= 1) {
		value -= 1;
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
	shift_byte(bin_to_7seg(gear, OFF));

	bcd_speed();
	shift_byte(bin_to_7seg(bcd_vect[2], OFF));
	shift_byte(bin_to_7seg(bcd_vect[1], OFF));
	shift_byte(bin_to_7seg(bcd_vect[0], OFF));

	shift_bar(revs_to_bar(), 22);
	shift_bar(temp_to_bar(), 10);

	shift_strobe();
}
