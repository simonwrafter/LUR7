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

#ifndef _CONFIG_H_
#define _CONFIG_H_

//Outputs
//! Output for Clutch Servo, PWM
#define CLUTCH				OUT1 //PWM on Timer1
//! Output for Gear Up signal to solenoid.
#define GEAR_UP				OUT4
//! Output for Gear Down signal to solenoid.
#define GEAR_DOWN			OUT5
//! Output for Shift cut (DTA pin 14)
#define SHIFT_CUT			OUT6

#endif // _CONFIG_H_
