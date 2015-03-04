/*
 * LUR7_adc.h - Sets up and retreives analog input values through a 10 bit ADC.
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

/*! \file LUR7_adc.h
 * \ref LUR7_adc sets up and retreives analog input values through 10 bit ADC for
 * the LUR7 project.
 * 
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 *
 * \see LUR7_adc
 * \see LUR7_adc.c
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \addtogroup LUR7_adc
 */

//see LUR7_adc.c for documentation

#ifndef _LUR7_ADC_H_
#define _LUR7_ADC_H_

void adc_init(void);

uint16_t adc_get(uint8_t);

#endif  // _LUR7_ADC_H_
