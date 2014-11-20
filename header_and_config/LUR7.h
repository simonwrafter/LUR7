/*
 * / LUR7.c - A collection of functions to setup and ease the use of the LUR7 PCB
 * / Copyright (C) 2014  Simon Wrafter <simon.wrafter@gmail.com>
 * / 
 * / This program is free software: you can redistribute it and/or modify
 * / it under the terms of the GNU General Public License as published by
 * / the Free Software Foundation, either version 3 of the License, or
 * / (at your option) any later version.
 * / 
 * / This program is distributed in the hope that it will be useful,
 * / but WITHOUT ANY WARRANTY; without even the implied warranty of
 * / MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * / GNU General Public License for more details.
 * / 
 * / You should have received a copy of the GNU General Public License
 * / along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <avr/io.h> 

const uint8_t NBR_OF_IO = 17;

//digital inputs
const uint8_t IN1 = 0;
const uint8_t IN2 = 1;
const uint8_t IN3 = 2;
const uint8_t IN4 = 3;
const uint8_t IN5 = 4;
const uint8_t IN6 = 5;
const uint8_t IN7 = 6;
const uint8_t IN8 = 7;
const uint8_t IN9 = 8;

//analog inputs
const uint8_t AN1 = 4;
const uint8_t AN2 = 6;

//digital outputs
const uint8_t OUT1 = 9;
const uint8_t OUT2 = 10;
const uint8_t OUT3 = 11;
const uint8_t OUT4 = 12;
const uint8_t OUT5 = 13;
const uint8_t OUT6 = 14;
const uint8_t OUT7 = 15;
const uint8_t OUT8 = 16;

// ADC
const uint8_t ADC_AN1 = 7;
const uint8_t ADC_AN2 = 4;
const uint8_t ADC_TEMP = 11;
const uint8_t ADC_SUPPLY = 0x10;

const uint8_t DDX[] = {
	DDRD, //IN1
	DDRD, //IN2
	DDRD, //IN3
	DDRB, //IN4
	DDRC, //IN5
	DDRB, //IN6
	DDRD, //IN7
	DDRB, //IN8
	DDRB, //IN9
	DDRB, //OUT1
	DDRB, //OUT2
	DDRD, //OUT3
	DDRC, //OUT4
	DDRC, //OUT5
	DDRC, //OUT6
	DDRB, //OUT7
	DDRB //OUT8
};

const uint8_t DDXn[] = {
	DDD3, //IN1
	DDD2, //IN2
	DDD1, //IN3
	DDB7, //IN4
	DDC0, //IN5
	DDB6, //IN6
	DDD7, //IN7
	DDB5, //IN8
	DDB2, //IN9
	DDB0, //OUT1
	DDB1, //OUT2
	DDD7, //OUT3
	DDC4, //OUT4
	DDC5, //OUT5
	DDC6, //OUT6
	DDB3, //OUT7
	DDB4 //OUT8
};

const uint8_t PORTX[] = {
	PORTD, //IN1
	PORTD, //IN2
	PORTD, //IN3
	PORTB, //IN4
	PORTC, //IN5
	PORTB, //IN6
	PORTD, //IN7
	PORTB, //IN8
	PORTB, //IN9
	PORTB, //OUT1
	PORTB, //OUT2
	PORTD, //OUT3
	PORTC, //OUT4
	PORTC, //OUT5
	PORTC, //OUT6
	PORTB, //OUT7
	PORTB //OUT8
};

const uint8_t PORTXn[] = {
	PORTD3, //IN1
	PORTD2, //IN2
	PORTD1, //IN3
	PORTB7, //IN4
	PORTC0, //IN5
	PORTB6, //IN6
	PORTD7, //IN7
	PORTB5, //IN8
	PORTB2, //IN9
	PORTB0, //OUT1
	PORTB1, //OUT2
	PORTD7, //OUT3
	PORTC4, //OUT4
	PORTC5, //OUT5
	PORTC6, //OUT6
	PORTB3, //OUT7
	PORTB4 //OUT8
};

const uint8_t PINX[] = {
	PIND, //IN1
	PIND, //IN2
	PIND, //IN3
	PINB, //IN4
	PINC, //IN5
	PINB, //IN6
	PIND, //IN7
	PINB, //IN8
	PINB, //IN9
	PINB, //OUT1
	PINB, //OUT2
	PIND, //OUT3
	PINC, //OUT4
	PINC, //OUT5
	PINC, //OUT6
	PINB, //OUT7
	PINB //OUT8
};

const uint8_t PINXn[] = {
	PIND3, //IN1
	PIND2, //IN2
	PIND1, //IN3
	PINB7, //IN4
	PINC0, //IN5
	PINB6, //IN6
	PIND7, //IN7
	PINB5, //IN8
	PINB2, //IN9
	PINB0, //OUT1
	PINB1, //OUT2
	PIND7, //OUT3
	PINC4, //OUT4
	PINC5, //OUT5
	PINC6, //OUT6
	PINB3, //OUT7
	PINB4 //OUT8
};



void init_io(void);
void init_adc(void);

uint8_t set_output(uint8_t, uint8_t);
uint8_t get_output(uint8_t);
uint8_t toggle_output(uint8_t);

uint8_t get_input(uint8_t);
uint16_t get_analog(uint8_t);