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

#ifndef _CONFIG_H_
#define _CONFIG_H_

//Inputs
//! Reset rear MCU
#define IO_RESET			IN2
//! Alternate
#define IO_ALT_BTN			IN3
//! Input for Log Start/Stop button
#define IO_CLUTCH_RIGHT		ADC_IN4
//! Input for Neutral Gear button
#define IO_GEAR_NEUTRAL		IN6
//! Input for Gear Down paddle
#define IO_GEAR_DOWN		IN8
//! Input for Gear Up paddle
#define IO_GEAR_UP			IN9

//External interrupts
//! Interrupt vector for Gear Up paddle
#define INT_GEAR_UP			INT_IN9_vect
//! Interrupt vector for Gear Down paddle
#define INT_GEAR_DOWN		INT_IN8_vect
//! Interrupt vector for Neutral Gear button
//#define INT_GEAR_NEUTRAL	INT_IN5_vect

#endif // _CONFIG_H_
