/*
 * shiftregister.c - populates shift registers
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

/*! \file shiftregister.c
 * \ref shiftregister populates the the siftregisters for the LED display used
 * in the LUR7.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref shiftregister.h
 * \see \ref main.c
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup shiftregister Shift Register
 * \ref shiftregister.c Populates shift registers.
 *
 * \see \ref shiftregister.c
 * \see \ref shiftregister.h
 * \see \ref display
 * \see \ref main
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "../header_and_config/LUR7.h"
#include "config.h"
#include "shiftregister.h"

//! Shifts out a byte.
/*!
 * Utilising the function \ref shift_bit a full byte is shifted out to the
 * shift registers.
 *
 * \param value the byte to shift out.
 */
void shift_byte(uint8_t value) {
	for (uint8_t i= 0; i<8; i++) {
		shift_bit(value & (1<<i));
	}
}

//! Shift out a bar.
/*!
 * Utilising the function \ref shift_bit a bar is shifted out to the shift
 * registers. \p nbr_high number of high bits are shifted out, followed by
 * \p length - \p nbr_high low bits.
 *
 * \param nbr_high the number of high bits to shift out.
 * \param length the total length of the bar
 */
void shift_bar(uint8_t nbr_high, uint8_t length) {
	uint8_t i=0;
	for (; i<nbr_high; i++) {
		shift_bit(ON);
	}
	for (; i<length; i++) {
		shift_bit(OFF);
	}
}

//! Shift out a single bit.
/*!
 * Shifts out a bit by setting the data output to \p value, waiting and then
 * triggering generating a clock pulse.
 *
 * \param value the bit value to shift out.
 */
void shift_bit(uint8_t value) {
	set_output(IO_SHIFT_DATA, value);
	_delay_us(PULSE_TIME);
	set_output(IO_SHIFT_CLK, HIGH);
	_delay_us(PULSE_TIME);
	set_output(IO_SHIFT_CLK, LOW);
	_delay_us(PULSE_TIME);
	set_output(IO_SHIFT_DATA, LOW);
}

//! Strobe the data through.
/*!
 * Data stored in the shift registers is latched through to the outputs of the
 * shift registers when strobe is set high and locked when it returnes to low.
 */
void shift_strobe(void) {
	_delay_us(PULSE_TIME);
	set_output(IO_SHIFT_STROBE, HIGH);
	_delay_us(PULSE_TIME);
	set_output(IO_SHIFT_STROBE, LOW);
}
