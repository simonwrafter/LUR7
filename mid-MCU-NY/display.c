/*
 * display.c - Controls the LED display of the LUR7
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

/*! \file display.c
 * \ref display controls the LED display used in the LUR7.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref display.h
 * \see \ref main.c
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup display Display Control
 * \ref display.c contains functions controling the LED display of the LUR7.
 *
 * \see \ref display.c
 * \see \ref display.h
 * \see \ref main
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "../header_and_config/LUR7.h"
#include "display.h"
#include "shiftregister.h"
#include "config.h"

//! Minimum engine revs
static const float REV_MIN = 2000;
//! Maximum engine revs
static const float REV_MAX = 11000;
//! Minimum number of LEDs for rev bar
static const float REV_BAR_MIN = 1;
//! Maximum number of LEDs for rev bar
static const float REV_BAR_MAX = 21;

//! Array of bit patterns for numbers on seven segment display.
static const uint8_t sev_seg[12] = {
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

//! Engine revs
static volatile uint16_t revs = 10999;
//! Current gear
static volatile uint8_t  gear = 10;
//! Current speed
static volatile uint16_t speed = 0;
//! Logging number
static volatile uint16_t log_id = 0;
//! Water temperature
static volatile uint16_t water_temp = 110;
//! Oil temperature
static volatile uint16_t oil_temp = 110;
//! BCD representation of number to display.
static volatile uint8_t  bcd_vect[3] = {0,0,0};

//! Set new RPM value.
void update_RPM(uint16_t new_RPM) {
	revs = new_RPM;
	if (revs > 2000) {
//		set_output(IO_START_BTN_LED, ON);
	} else {
//		set_output(IO_START_BTN_LED, OFF);
	}
}

//! Set new current gear.
void update_gear(uint8_t new_gear) {
	gear = new_gear;
}

//! Set new current speed.
void update_speed(uint16_t new_speed) {
	speed = new_speed;
}

//! Set new loggin number.
void update_logid(uint16_t new_logid) {
	log_id = new_logid;
}

//! Set new water temperature.
void update_watertemp(uint16_t new_watertemp) {
	water_temp = new_watertemp;
}

//! Set new oil temperature.
void update_oiltemp(uint16_t new_oiltemp) {
	oil_temp = new_oiltemp;
}

//! Get the current gear.
uint8_t get_current_gear() {
	return gear;
}

//! Calculate the number of LEDs to light for the rev-bar.
/*!
 * The Rev-Bar is lit up in a linear manner proportional to the engine revs.
 */
uint8_t revs_to_bar() {
	float return_val = (revs - REV_MIN) / (REV_MAX - REV_MIN) * REV_BAR_MAX + REV_BAR_MIN;
	if (return_val < REV_BAR_MIN) {
		return 0;
	} else if (return_val > REV_BAR_MAX) {
		return REV_BAR_MAX;
	}
	return return_val;
}

//! Determine the number of LEDs to light for the temperature bar.
/*!
 * The Temperature Bar is lit up in a alinear manner. The intervalls are chosen
 * to give good resolution in the interesting regions, while maintaining a wide
 * span. \ref TEMP_LVL_1 to \ref TEMP_LVL_10 determine the steps.
 */
uint8_t temp_to_bar(uint16_t temperature) {
	if (temperature < TEMP_LVL_1) { // 20
		return 0;
	} else if (temperature < TEMP_LVL_2) { // 40
		return 1;
	} else if (temperature < TEMP_LVL_3) { // 50
		return 2;
	} else if (temperature < TEMP_LVL_4) { // 60
		return 3;
	} else if (temperature < TEMP_LVL_5) { // 70
		return 4;
	} else if (temperature < TEMP_LVL_6) { // 80
		return 5;
	} else if (temperature < TEMP_LVL_7) { // 85
		return 6;
	} else if (temperature < TEMP_LVL_8) { // 90
		return 7;
	} else if (temperature < TEMP_LVL_9) { // 95
		return 8;
	} else if (temperature < TEMP_LVL_10) { // 100
		return 9;
	}
	return 10;
}

//! Convert binary to BCD
/*!
 * Calculates the Binary Coded Decimal representation of \p value.
 *
 * \param value number to convert.
 */
void bcd_convert(uint16_t value) {
	if (value >= 100) { // if larger than 100
		bcd_vect[0] = 0; // reset to 0
		while (value >= 100) { // while larger than 99
			value -= 100; // reduce by 100
			bcd_vect[0]++; // count the hundreds
		}
	} else {
		bcd_vect[0] = 10; // clear digit
	}

	if (value >= 10) { // if larger than 10
		bcd_vect[1] = 0; // reset to 0
		while (value >= 10) { // while larger than 9
			value -= 10; // reduce by 10
			bcd_vect[1]++; //count the tens
		}
	} else {
		if (bcd_vect[0] == 10) { // if first digit is clear
			bcd_vect[1] = 10; // clear digit
		} else {
			bcd_vect[1] = 0; // else set to 0
		}
	}

	bcd_vect[2] = 0; // reset to 0
	while (value >= 1) { // while larger than 0
		value -= 1; // reduce by 1
		bcd_vect[2]++; // count the ones
	}
}

//! Get seven segment representation of number
/*!
 * For binary numbers [0, 9] this function returns the seven segment
 * representation of the number with an optional decimal point.
 *
 * \param binary number to convert.
 * \param dp decimal point.
 */
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

//! Updates the display
/*!
 * Re-populates the shift registers with the latest information available.
 */

/*
void update_display(uint8_t mode) {
	shift_byte(bin_to_7seg(gear, OFF));
	//shift_bar(0, 56);
	shift_bar(0, 24);
	
	//if (mode) {
	//	bcd_convert(speed);
	//} else {
	//	bcd_convert(speed);
	//}
	//shift_byte(bin_to_7seg(bcd_vect[0], OFF));
	//shift_byte(bin_to_7seg(bcd_vect[1], OFF));
	//shift_byte(bin_to_7seg(bcd_vect[2], OFF));
	
	shift_bar(revs_to_bar(), REV_BAR_MAX);
	
	shift_bar(0, 10);
	
	//if (mode) {
	//	shift_bar(temp_to_bar(water_temp), 10);
	//} else {
	//	shift_bar(temp_to_bar(oil_temp), 10);
	//}
	
	// STROBE
	shift_strobe();
}
*/
void update_display(uint8_t mode) {
	shift_byte(bin_to_7seg(gear, OFF));
	shift_bar(0, 56);
	
	// STROBE
	shift_strobe();
}
