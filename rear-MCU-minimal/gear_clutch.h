/*
 * gear_clutch.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file gear_clutch.h
 * The \ref gear_clutch defines inputs and constants used on the rear MCU.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref gear_clutch
 * \see \ref gear_clutch.c
 * \see \ref main.c
 * \see \ref config.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \addtogroup gear_clutch
 */

#ifndef _GEAR_CLUTCH_H_
#define _GEAR_CLUTCH_H_

void gear_up(void);

void gear_down(void);

void gear_neutral(uint8_t);

void clutch_set(uint16_t);

#endif // _GEAR_CLUTCH_H_
