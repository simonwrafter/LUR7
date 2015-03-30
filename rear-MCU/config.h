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
 * The \ref config defines inputs and constants used on the rear MCU.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref config
 * \see \ref main.c
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup config Rear MCU - Configuration File
 * The rear MCU has several inputs for measuring the behaviour of the LUR7 and
 * outputs for gear changing. In \ref config.h the setup of the MCU is defined.
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

//! The value above which the brakes are considered to have been applied.
#define BRAKES_ON	100 // MUST above BRAKES_OFF
//! The value below which the brakes are considered to have been released.
#define BRAKES_OFF	80 // MUST below BRAKES_ON

//Inputs
//! Input for speed measurment of rear right wheel.
#define WHEEL_R				IN1
//! Input for speed measurment of rear left wheel.
#define WHEEL_L				IN2
//! Backup, Gear Up button. (see middle MCU)
#define BAK_IN_GEAR_UP		IN3
//! Backup, Brake Pressure Sensor. (see front MCU)
#define BAK_IN_BRAKE	ADC_IN4
//! Backup, Gear Down button. (see middle MCU)
#define BAK_IN_GEAR_DOWN	IN5
//! Backup, Clutch Paddle angle sensor. (see middle MCU)
#define BAK_IN_CLUTCH	ADC_IN6
//! Backup, Neutral Gear button. (see middle MCU)
#define BAK_IN_NEUTRAL		IN7
//! Input (analog) for measurment of rear right suspension.
#define SUSPENSION_R	ADC_IN8
//! Input (analog) for measurment of rear left suspension.
#define SUSPENSION_L	ADC_IN9


//Outputs
//! Output for Clutch Servo, PWM
#define CLUTCH				OUT1 //PWM on Timer1
//unused 					OUT2
//! Output for Brakelight
#define BRAKE_LIGHT			OUT3
//! Output for Gear Up signal to solenoid.
#define GEAR_UP				OUT4
//! Output for Gear Down signal to solenoid.
#define GEAR_DOWN			OUT5
//! Output for Shift cut (DTA pin 14)
#define SHIFT_CUT			OUT6
//! Output for controling Launch Control.
#define LAUNCH				OUT7
//! Output to control availability of ground for sensors.
#define GND_CONTROL			OUT8


//External interrupts
//none

#endif // _CONFIG_H_
