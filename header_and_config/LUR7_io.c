/*
/ LUR7_io.c - A collection of functions to setup and ease the use of the LUR7 PCB
/ Copyright (C) 2014  Simon Wrafter <simon.wrafter@gmail.com>
/
/ This program is free software: you can redistribute it and/or modify
/ it under the terms of the GNU General Public License as published by
/ the Free Software Foundation, either version 3 of the License, or
/ (at your option) any later version.
/
/ This program is distributed in the hope that it will be useful,
/ but WITHOUT ANY WARRANTY; without even the implied warranty of
/ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/ GNU General Public License for more details.
/
/ You should have received a copy of the GNU General Public License
/ along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*! \file LUR7_io.c
 * \ref LUR7_io provides functions for setting up and using the digital inputs
 * and outputs of the LUR7 PCB. 
 * 
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 * 
 * \see LUR7_io
 * \see LUR7_io.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 *
 * \defgroup LUR7_io In- and Outputs
 * The aim of this module is to simplify the reading of input values and writing
 * to outputs. By removing the setting of bits in registers from the main code
 * and handling it all in this controlled environment reduces the risk of faulty
 * pin settings.
 * 
 * The initialisation function ensures that only the pins that are meant to be
 * used as outputs are configured as such, and the same goes for inputs.
 * 
 * The remaining functions are straight forward in their use. The arguments to
 * the functions are defined in \ref LUR7.h as \ref IN1 - \ref IN9 and \ref OUT1 - \ref OUT8.
 * 
 * \see LUR7_io.c
 * \see LUR7_io.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 */


#include <avr/cpufunc.h> //included for _NOP()
#include "LUR7.h"
#include "LUR7_io.h"

// vectors of ports as used on PCB, used localy only!
//! Data Direction Register for the respective in- and outputs
static volatile uint8_t * DDX[19] = {
	&DDRD, //IN1
	&DDRD, //IN2
	&DDRD, //IN3
	&DDRB, //IN4
	&DDRC, //IN5
	&DDRB, //IN6
	&DDRD, //IN7
	&DDRB, //IN8
	&DDRB, //IN9
	&DDRC, //OUT1 MCU v1.1
//	&DDRB, //OUT1 MCU v1.0
	&DDRB, //OUT2
	&DDRD, //OUT3
	&DDRC, //OUT4
	&DDRC, //OUT5
	&DDRC, //OUT6
	&DDRB, //OUT7
	&DDRB  //OUT8
};

//! Data Direction Register bits for the respective in- and outputs
static uint8_t DDXn[19] = {
	DDD3, //IN1
	DDD2, //IN2
	DDD1, //IN3
	DDB7, //IN4
	DDC0, //IN5
	DDB6, //IN6
	DDD0, //IN7
	DDB5, //IN8
	DDB2, //IN9
	DDC1, //OUT1 MCU v1.1
//	DDB0, //OUT1 MCU v1.0
	DDB1, //OUT2
	DDD7, //OUT3
	DDC4, //OUT4
	DDC5, //OUT5
	DDC6, //OUT6
	DDB3, //OUT7
	DDB4  //OUT8
};

//! Port Data register for the respective in- and outputs
static volatile uint8_t * PORTX[19] = {
	
	&PORTD, //IN1
	&PORTD, //IN2
	&PORTD, //IN3
	&PORTB, //IN4
	&PORTC, //IN5
	&PORTB, //IN6
	&PORTD, //IN7
	&PORTB, //IN8
	&PORTB, //IN9
	&PORTC, //OUT1 MCU v1.1
//	&PORTB, //OUT1 MCU v1.0
	&PORTB, //OUT2
	&PORTD, //OUT3
	&PORTC, //OUT4
	&PORTC, //OUT5
	&PORTC, //OUT6
	&PORTB, //OUT7
	&PORTB  //OUT8
};

//! Port Data register bits for the respective in- and outputs
static uint8_t PORTXn[19] = {
	PORTD3, //IN1
	PORTD2, //IN2
	PORTD1, //IN3
	PORTB7, //IN4
	PORTC0, //IN5
	PORTB6, //IN6
	PORTD0, //IN7
	PORTB5, //IN8
	PORTB2, //IN9
	PORTC1, //OUT1 MCU v1.1
//	PORTB0, //OUT1 MCU v1.0
	PORTB1, //OUT2
	PORTD7, //OUT3
	PORTC4, //OUT4
	PORTC5, //OUT5
	PORTC6, //OUT6
	PORTB3, //OUT7
	PORTB4  //OUT8
};

//! Port Input Pins register for the respective in- and outputs
static volatile uint8_t * PINX[19] = {
	&PIND, //IN1
	&PIND, //IN2
	&PIND, //IN3
	&PINB, //IN4
	&PINC, //IN5
	&PINB, //IN6
	&PIND, //IN7
	&PINB, //IN8
	&PINB, //IN9
	&PINC, //OUT1 MCU v1.1
//	&PINB, //OUT1 MCU v1.0
	&PINB, //OUT2
	&PIND, //OUT3
	&PINC, //OUT4
	&PINC, //OUT5
	&PINC, //OUT6
	&PINB, //OUT7
	&PINB  //OUT8
};

//! Port Input Pins register bits for the respective in- and outputs
static uint8_t PINXn[19] = {
	PIND3, //IN1
	PIND2, //IN2
	PIND1, //IN3
	PINB7, //IN4
	PINC0, //IN5
	PINB6, //IN6
	PIND0, //IN7
	PINB5, //IN8
	PINB2, //IN9
	PINC1, //OUT1 MCU v1.1
//	PINB0, //OUT1 MCU v1.0
	PINB1, //OUT2
	PIND7, //OUT3
	PINC4, //OUT4
	PINC5, //OUT5
	PINC6, //OUT6
	PINB3, //OUT7
	PINB4  //OUT8
};

//! Hardware initialisation function.
/*!
 * The Data Direction Registers for the outputs are configured. Run this 
 * function during setup to use the outputs.
 */
void io_init(void) {
	for (uint8_t i = FIRST_OUT; i <= LAST_OUT; i++) { // for each output
		*DDX[i] |= (1<<DDXn[i]); // set DDXn=1 for all outputs
	}
	MCUCR |= (1 << PUD); // disable Pull-up resistors on chip.
}

//other functions

//! Set output value for port.
/*!
 * A function to set output \p port to the value \p data.
 * 
 * \ref LUR7.h contains definitions of ports and useful parameter values for \p data.
 * 
 * \see LUR7.h
 * 
 * \param port The output to set value for.
 * \param data The value to output on \p port.
 * \return The value outputed on \p port. 0xFF if \p port is not an output.
 */
uint8_t set_output(uint8_t port, uint8_t data) {
	if (port >= FIRST_OUT && port <= LAST_OUT) { // check that port is an output
		if (!data) { // invert because of pull-up resistors
			*PORTX[port] |= (1 << PORTXn[port]); // set output to one
			return 1; // return output value
		} else {
			*PORTX[port] &= ~(1 << PORTXn[port]); // set output to 0
			return 0; // return output value
		}
	}
	return 0xFF; // return 0xFF if port is not an output
}

//! Get current output value.
/*!
 * Retreives the current value of the output \p port.
 *
 * \ref LUR7.h contains definitions of ports.
 * 
 * \param port The output to read value for.
 * \return The output value of \p port. 0xFF if \p port is not an output.
 */
uint8_t get_output(uint8_t port) {
	if (port >= FIRST_OUT && port <= LAST_OUT) { // check that port is an output
		return !get_input(port); // same method as for inputs, invert for pull-up
	}
	return 0xFF; // return 0xFF if port i not an output
}

//! Invert current output value.
/*!
 * A function to invert the output value on \p port.
 *
 * \ref LUR7.h contains definitions of ports.
 * 
 * \param port The output to toggle the value for.
 * \return The output value of \p port after toggle. 0xFF if \p port is not an output.
 */
uint8_t toggle_output(uint8_t port) {
	if (port >= FIRST_OUT && port <= LAST_OUT) {
		*PINX[port] = (1 << PINXn[port]);
		_NOP(); //FIXME: concidered bad, other options for very short delays?
		return get_output(port);
	}
	return 0xFF;
}

//! Get input value of input.
/*!
 * Retreives the current value of the input \p port.
 *
 * \ref LUR7.h contains definitions of ports.
 * 
 * \param port The input to read value for.
 * \return The input value of \p port. 0xFF if \p port is not an input.
 */
uint8_t get_input(uint8_t port) {
	if (port >= FIRST_IN && port <= LAST_IN) {
		return (*PINX[port] & (1 << PINXn[port]) ? 1 : 0);
	}
	return 0xFF;
}
