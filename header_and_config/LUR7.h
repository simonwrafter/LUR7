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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

// CLOCK SETTINGS AND DELAY
#define FOSC           16000        // 16Mhz external clock
#define F_CPU          16000000     // Needed for delay
#include <util/delay.h>

// CAN LIB CONFIGURATION
#define CAN_BAUDRATE   100        // in kBit

// SYSTEM DEFINITION
//digital inputs
#define IN1 0 //PD3
#define IN2 1 //PD2
#define IN3 2 //PD1
#define IN4 3 //PB7
#define IN5 4 //PC0
#define IN6 5 //PB6
#define IN7 6 //PD0
#define IN8 7 //PB5
#define IN9 8 //PB2

//analog inputs
#define AN1 4 //PC0
#define AN2 6 //PD0
#define AN3 7 //PB5
#define AN4 8 //PB2

//digital outputs
#define OUT1 9  //PB0
#define OUT2 10 //PB1
#define OUT3 11 //PD7
#define OUT4 12 //PC4
#define OUT5 13 //PC5
#define OUT6 14 //PC6
#define OUT7 15 //PB3
#define OUT8 16 //PB4

// ADC
#define ADC_AN1 0x04 //PC0
#define ADC_AN2 0x07 //PD0
#define ADC_AN3 0x06 //PB5
#define ADC_AN4 0x05 //PB2
#define ADC_TEMP 0x0B
#define ADC_SUPPLY 0x10

// INTERRUPTS




// SYSTEM FUNCTIONS
#include "LUR7_io.h"
#include "LUR7_interrupt.h"

#endif  // _LUR7_H_
