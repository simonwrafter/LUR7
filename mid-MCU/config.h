/*
 * config.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file config.h
 * The \ref config defines inputs and constants used on the mid MCU.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref config
 * \see \ref main.c
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup config mid MCU - Configuration File
 * The mid MCU has several inputs for measuring the behaviour of the LUR7. In
 * \ref config.h the setup of the MCU is defined.
 *
 * \see \ref config.h
 * \see \ref main
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

//Constants
//! The number of clutch readings to average
#define CLUTCH_ATOMIC_LENGTH	10

//Inputs
//unused					IN1
//#define					IN2
//! Input for General Purpose button
#define IO_ALT_BTN			IN3
//! Input for Log Start/Stop button
#define IO_LOG_BTN			IN4
//! Input for Neutral Gear button
#define IO_GEAR_NEUTRAL		IN5
//! Input for Clutch paddle
#define IO_CLUTCH		ADC_IN6
//#define					IN7
//! Input for Gear Down paddle
#define IO_GEAR_DOWN		IN8
//! Input for Gear Up paddle
#define IO_GEAR_UP			IN9

//Outputs
//unused					OUT1
//!Outout for Shift Register Strobe
#define IO_SHIFT_STROBE		OUT2
//!Outout for Shift Register Data
#define IO_SHIFT_DATA		OUT3
//!Outout for Shift Register Clock
#define IO_SHIFT_CLK		OUT4
//unused					OUT5
//unused					OUT6
//unused					OUT7
//unused					OUT8


//External interrupts
//! Interrupt vector for Gear Up paddle
#define INT_GEAR_UP			INT_IN9_vect
//! Interrupt vector for Gear Down paddle
#define INT_GEAR_DOWN		INT_IN8_vect
//! Interrupt vector for Neutral Gear button
#define INT_GEAR_NEUTRAL	INT_IN5_vect

#endif // _CONFIG_H_
