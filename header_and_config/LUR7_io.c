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

#include <avr/cpufunc.h> //included for _NOP()
#include "LUR7.h"
#include "LUR7_io.h"

// vectors of ports as used on PCB, used localy only!
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
//#ifdef PCBv11
//	&DDRC, //OUT1 MCU v1.1
//#else
	&DDRB, //OUT1 MCU v1.0
//#endif
	&DDRB, //OUT2
	&DDRD, //OUT3
	&DDRC, //OUT4
	&DDRC, //OUT5
	&DDRC, //OUT6
	&DDRB, //OUT7
	&DDRB  //OUT8
};

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
//#ifdef PCBv11
//	DDC1, //OUT1 MCU v1.1
//#else
	DDB0, //OUT1 MCU v1.0
//#endif
	DDB1, //OUT2
	DDD7, //OUT3
	DDC4, //OUT4
	DDC5, //OUT5
	DDC6, //OUT6
	DDB3, //OUT7
	DDB4  //OUT8
};

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
//#ifdef PCBv11
//	&PORTC, //OUT1 MCU v1.1
//#else
	&PORTB, //OUT1 MCU v1.0
//#endif
	&PORTB, //OUT2
	&PORTD, //OUT3
	&PORTC, //OUT4
	&PORTC, //OUT5
	&PORTC, //OUT6
	&PORTB, //OUT7
	&PORTB  //OUT8
};

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
//#ifdef PCBv11
//	PORTC1, //OUT1 MCU v1.1
//#else
	PORTB0, //OUT1 MCU v1.0
//#endif
	PORTB1, //OUT2
	PORTD7, //OUT3
	PORTC4, //OUT4
	PORTC5, //OUT5
	PORTC6, //OUT6
	PORTB3, //OUT7
	PORTB4  //OUT8
};

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
//#ifdef PCBv11
//	&PINC, //OUT1 MCU v1.1
//#else
	&PINB, //OUT1 MCU v1.0
//#endif
	&PINB, //OUT2
	&PIND, //OUT3
	&PINC, //OUT4
	&PINC, //OUT5
	&PINC, //OUT6
	&PINB, //OUT7
	&PINB  //OUT8
};

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
//#ifdef PCBv11
//	PINC1, //OUT1 MCU v1.1
//#else
	PINB0, //OUT1 MCU v1.0
//#endif
	PINB1, //OUT2
	PIND7, //OUT3
	PINC4, //OUT4
	PINC5, //OUT5
	PINC6, //OUT6
	PINB3, //OUT7
	PINB4  //OUT8
};

//Init function

void io_init(void) {
	for (uint8_t i = OUT1; i<NBR_OF_IO; i++) { //set DDXn=1 for all outputs
		*DDX[i] |= (1<<DDXn[i]);
	}
}

//other functions

uint8_t set_output(uint8_t port, uint8_t data) {
	if (!data) {
		*PORTX[port] |= (1 << PORTXn[port]);
		return 1;
	} else {
		*PORTX[port] &= ~(1 << PORTXn[port]);
		return 0;
	}
}

uint8_t get_output(uint8_t port) {
	return !get_input(port);
}

uint8_t toggle_output(uint8_t port){
	*PINX[port] = (1 << PINXn[port]);
	_NOP(); //FIXME: concidered bad, other options for very short delays?
	return get_input(port);
}

uint8_t get_input(uint8_t port) {
	return (*PINX[port] & (1 << PINXn[port]) ? 1 : 0);
}
