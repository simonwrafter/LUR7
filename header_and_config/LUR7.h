/*
 * LUR7.h - The main .h file for the LUR7 project. Include this file in main.c.
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

/*! \file LUR7.h
 * LUR7.h is the main .h file for the entire LUR7 project.
 *
 * All code is released under the GPLv3 license.
 *
 * To write code for the LUR7 PCB only this file should be included to each new
 * source file, all other dependencies are included from here.
 *
 * \see LUR7
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup LUR7 Shared - Main header file
 * In LUR7.h a number of global macros are defined for inputs and outputs,
 * system clock, CAN baudrate etc.
 *
 * All dependencies are included through this file, this means that to write
 * code for the ATmega32M1 and LUR7 PCB only this file needs to be included to
 * each source file.
 *
 * \see LUR7.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#ifndef _LUR7_H_
#define _LUR7_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdint.h>

// CLOCK SETTINGS AND DELAY
//! The clockspeed of the system
#define F_CPU	16000000UL     // 16Mhz external clock, for delay and CAN
#include <util/delay.h>

// CAN LIB CONFIGURATION
//! The baudrate at which the CAN bus operates, must be identical for all attached units.
#define CAN_BAUDRATE	1000        // in kBit

// SYSTEM DEFINITION

//! IN1 is defined as PD3 and used as an input on the PCB.
#define IN1		0 //PD3
//! IN2 is defined as PD2 and used as an input on the PCB.
#define IN2		1 //PD2
//! IN3 is defined as PD1 and used as an input on the PCB.
#define IN3		2 //PD1
//! IN4 is defined as PB7 and used as an input on the PCB.
#define IN4		3 //PB7
//! IN5 is defined as PC0 and used as an input on the PCB.
#define IN5		4 //PC0
//! IN6 is defined as PB6 and used as an input on the PCB.
#define IN6		5 //PB6
//! IN7 is defined as PD0 and used as an input on the PCB.
#define IN7		6 //PD0
//! IN8 is defined as PB5 and used as an input on the PCB.
#define IN8		7 //PB5
//! IN9 is defined as PB2 and used as an input on the PCB.
#define IN9		8 //PB2

//digital outputs
//! OUT1 is defined as PC1 and used as an output on the PCB.
#define OUT1	9  //PB0 v1.0; PC1 v1.1
//! OUT2 is defined as PB1 and used as an output on the PCB.
#define OUT2	10 //PB1
//! OUT3 is defined as PD7 and used as an output on the PCB.
#define OUT3	11 //PD7
//! OUT4 is defined as PC4 and used as an output on the PCB.
#define OUT4	12 //PC4
//! OUT5 is defined as PC5 and used as an output on the PCB.
#define OUT5	13 //PC5
//! OUT6 is defined as PC6 and used as an output on the PCB.
#define OUT6	14 //PC6
//! OUT7 is defined as PB3 and used as an output on the PCB.
#define OUT7	15 //PB3
//! OUT8 is defined as PB4 and used as an output on the PCB.
#define OUT8	16 //PB4

//! Copy of the first input
#define FIRST_IN	IN1
//! Copy of the last input
#define LAST_IN		IN9
//! Copy of the first output
#define FIRST_OUT	OUT1
//! Copy of the last output
#define LAST_OUT	OUT8

// ADC
//! selects IN4 for A/D conversion
#define ADC_IN4			0x04 //PB7, IN4
//! selects IN6 for A/D conversion.
#define ADC_IN6			0x07 //PB6, IN6
//! selects IN8 for A/D conversion.
#define ADC_IN8			0x06 //PB5, IN8
//! selects IN9 for A/D conversion.
#define ADC_IN9			0x05 //PB2, IN9
//! selects the temperature sensor of the ATmega32M1 for A/D conversion.
#define ADC_TEMP		0x0B
//! selects the voltage over the capacitor bank on the PCB for A/D conversion.
#define ADC_SUPPLY_P	0x02 //PD5, Capacitor bank
//! selects the incoming voltage from the main power supply for A/D conversion.
#define ADC_SUPPLY_N	0x03 //PD6, 12V main

// LOGIC
//! logic TRUE */
#define TRUE	1
//! Eg. for setting outputs
#define HIGH	1
//! Eg. for setting outputs
#define ON		1
//! Eg. for setting outputs to a high impedance state
#define TRI		1
//! Eg. for setting outputs to a high impedance state
#define CUT		1

//! logic FALSE */
#define FALSE	0
//! Eg. for setting outputs
#define LOW		0
//! Eg. for setting outputs
#define OFF		0
//! Eg. for setting outputs to a low impedance state
#define GND		0
//! Eg. for setting outputs to a low impedance state
#define OPEN	0

// SYSTEM FUNCTIONS
#include "LUR7_io.h"
#include "LUR7_adc.h"
#include "LUR7_ancomp.h"
#include "LUR7_can.h"
#include "LUR7_interrupt.h"
#include "LUR7_power.h"
#include "LUR7_timer0.h"
#include "LUR7_timer1.h"

#endif  // _LUR7_H_
