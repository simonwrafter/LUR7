/*
 * / LUR7.h - A collection of functions to setup and ease the use of the LUR7 PCB
 * / Copyright (C) 2015  Simon Wrafter <simon.wrafter@gmail.com>
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

#ifndef _LUR7_H_
#define _LUR7_H_

//_____ I N C L U D E S ________________________________________________________
#include <avr/io.h>
#include <avr/interrupt.h>
#include "common.h"

//_____ D E F I N I T I O N S __________________________________________________

    // -------------- MCU LIB CONFIGURATION
#define FOSC           8000        // 8Mhz external clock
#define F_CPU          8000000     // Need for AVR GCC

#include <util/delay.h>

    // -------------- CAN LIB CONFIGURATION
#define CAN_BAUDRATE   100        // in kBit

    // -------------- MISCELLANEOUS

//_____ E N D __________________________________________________________________

#endif  // _LUR7_H_
