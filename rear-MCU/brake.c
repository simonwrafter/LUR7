/*
 * brake.c - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file brake.c
 * \ref brake controls braking of the LUR7.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref brake
 * \see \ref brake.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup brake Brake Control
 * \ref brake.c controls braking of the LUR7.
 *
 * \see \ref brake.c
 * \see \ref brake.h1
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "../header_and_config/LUR7.h"
#include "config.h"
#include "gear_clutch.h"

//! Turns brake light on or off.
/*!
 * The brake light is a mandatory component. When brake pressure rises over a 
 * the \ref BRAKES_ON level in the hydraulic system the light is turned on. A
 * small hysterisis is implemented turning the light of at the slightly lower 
 * \ref BRAKES_OFF level.
 * 
 * \param brake_pressure The pressure in the brake system.
 */
void brake_light(uint16_t brake_pressure) {
	if (brake_pressure > BRAKES_ON) {
		set_output(BRAKE_LIGHT, GND);
	} else if (brake_pressure < BRAKES_OFF) {
		set_output(BRAKE_LIGHT, TRI);
	}
}