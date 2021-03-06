/*
 * config.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file config.h
 * The \ref config defines inputs and constants used on the front MCU.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref config
 * \see \ref main.c
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup config Front MCU - Configuration File
 * The front MCU has several inputs for measuring the behaviour of the LUR7. In
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

//Inputs
//! Input for speed measurment of front right wheel.
#define WHEEL_R				IN1
//! Input for speed measurment of front left wheel.
#define WHEEL_L				IN2
// unused					IN3
//! Input (analog) for measurment of brake pressure.
#define BRAKE_PRESSURE	ADC_IN4
// unused					IN5
//! Input (analog) for measurment of steering wheel angle.
#define STEERING_WHEEL	ADC_IN6
// unused					IN7
//! Input (analog) for measurment of front right suspension.
#define SUSPENSION_R	ADC_IN8
//! Input (analog) for measurment of front left suspension.
#define SUSPENSION_L	ADC_IN9

//Outputs
//! Output to control ground for sensors.
#define GND_CONTROL			OUT8

//External interrupts
//none

#endif // _CONFIG_H_
