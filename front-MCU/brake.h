/*
 * brake.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file brake.h
 * \ref brake controls braking of the LUR7.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref brake
 * \see \ref brake.c
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \addtogroup brake
 */

#ifndef _BRAKE_H_
#define _BRAKE_H_

//! The value above which the brakes are considered to have been applied.
#define BRAKES_ON	100 // MUST above BRAKES_OFF
//! The value below which the brakes are considered to have been released.
#define BRAKES_OFF	80 // MUST below BRAKES_ON

uint8_t * brake_light(uint16_t);

#endif // _BRAKE_H_