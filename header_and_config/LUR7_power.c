/*
 / LUR7_power.c - A coll*ection of functions to setup and ease the use of the LUR7 PCB
 / Copyright (C) 2014  Simon Wrafter <simon.wrafter@gmail.com>
 /
 / This program is free software: you can redistribute it and/or modify
 / it under the terms of the GNU General Public License as published by
 / the Free Software Foundation, either version 3 of the License, or
 / (at your option) any later version.
 /
 / This program is distributed in the hope that it will be useful,
 / but WITHOUT ANY WARRANTY; without even the implied warranty of
 / MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 / GNU General Public License for more details.
 /
 / You should have received a copy of the GNU General Public License
 / along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file LUR7_power.c
 * \ref LUR7_power allows for unused peripheral hardware to be shut off, thus
 * reducing the overall power consumption.
 * 
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 *
 * \see LUR7_power
 * \see LUR7_power.h
 * \see http://www.gnu.org/copyleft/gpl.html
 * 
 * \defgroup LUR7_power Power Reduction Settings
 * 
 * \ref LUR7_power allows for unused peripheral hardware to be shut off, thus
 * reducing the overall power consumption.
 *
 * \see LUR7_power.c
 * \see LUR7_power.h
 * \see http://www.gnu.org/copyleft/gpl.html
 */

#include "LUR7.h"
#include "LUR7_power.h"

//! Default power settings
/*!
 * Shuts off power to the Power Stage Controller, SPI, and LIN interfaces. These
 * peripherals are never used and can thus be safely dissabled.
 */
void power_off_default(void) {
	PRR = (1 << PRPSC) | (1 << PRSPI) | (1 << PRLIN);
}

//! Dissable Timer1
/*! If \ref LUR7_timer1 is unused (no PWM output on \ref OUT1), this function
 * disables the \ref LUR7_timer1 hardware.
 */
void power_off_timer1(void) {
	PRR |= (1 << PRTIM1);
}
