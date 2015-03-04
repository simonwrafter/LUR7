/*
 * LUR7_adc.c - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file LUR7_adc.c
 * \ref LUR7_adc sets up and retreives analog input values through 10 bit ADC for
 * the LUR7 project.
 * 
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 *
 * \see LUR7_adc
 * \see LUR7_adc.h
 * \see http://www.gnu.org/copyleft/gpl.html
 *
 * \defgroup LUR7_adc A/D conversion
 * In this module functions pertaining to the initialization and usage of the 10
 * bit ADC are defined. Possible analog values for conversion are
 * - ADC_IN4
 *   + PB7
 * - ADC_IN6
 *   + PB6
 * - ADC_IN8
 *   + PB5
 * - ADC_IN9
 *   + PB2
 * - ADC_TEMP
 *   + internal temperature sensor
 * - ADC_SUPPLY_P
 *   + PD5, Capacitor bank
 * - ADC_SUPPLY_N
 *   + PD6, 12V main
 *
 * as defined in LUR7.h.
 *
 * To start using the ADC run adc_init(void) once. To get an analog read out run
 * adc_get(uint8_t) for each acquisition.
 *
 * Conversion does not use interrupts to return the result, meaning there is a
 * time delay from starting a conversion before returning the result.
 *
 * \see LUR7_adc.c
 * \see LUR7_adc.h
 * \see http://www.gnu.org/copyleft/gpl.html
 */

#include "LUR7.h"
#include "LUR7_adc.h"

//! Hardware initialisation function.
/*!
 * The ADC is started in single conversion mode. AVcc is used as voltage reference.
 * Interrupts are not used. The ADC is clocked at 2MHz (assuming 16 MHz crystal).
 *
 * To use the ADC, run this function once at boot time.
 */
void adc_init(void) {
	ADMUX = (1<<REFS0) | (1<<MUX3) | (1<<MUX1) | (1<<MUX0); //AVcc reference, temperature input (not converted)
	ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0); //enable hw, prescaler = 8 => 2MHz adc clock (max)
	ADCSRB = (1<<ADHSM); // high speed mode
	DIDR0 = (1 << ADC2D) | (1 << ADC3D); // disable digital buffer for ADC_SUPPLY_{N,P}
}

//! Analog value acquisition.
/*!
 * This function requests a conversion of an analog voltage level, waits for the
 * conversion to complete and returns the 10 bit result.
 *
 * Due to not using interrupts for returning converted values, there is a slight
 * delay in (pos. 100µs?) the acquisition time. Therefore requeting analog values
 * should be done sparingly. However, merely waiting for the result in a loop does
 * not hinder other interrupts to happen, it just slows down the execution of the
 * main code.
 *
 * \param analog_port The port to read analog level from
 * \return 10 bit representation of the analog value
 */
uint16_t adc_get(uint8_t analog_port) {
	ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
	ADMUX |= analog_port;
	if (analog_port < 8) {
		DIDR0 |= (1 << analog_port);
	}
	ADCSRA |= (1<<ADSC);

	while ((ADCSRA & (1<<ADSC))) {
		;
	}

	if (analog_port < 8) {
		DIDR0 &= ~(1 << analog_port);
	}
	return ADCL | (ADCH<<8);
}
