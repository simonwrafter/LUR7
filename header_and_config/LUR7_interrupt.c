/*
 * LUR7_interrupt.c - A collection of functions to setup and ease the use of the LUR7 PCB
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

#include "LUR7.h"
#include "LUR7_interrupt.h"

//EXTERNAL INTERRUPTS
static uint8_t ISCn0[9] = {
	0,     //IN1
	0,     //IN2
	0,     //IN3
	0,     //IN4
	ISC30, //IN5
	0,     //IN6
	0,     //IN7
	ISC20, //IN8
	ISC10, //IN9
};

static uint8_t ISCn1[9] = {
	0,     //IN1
	0,     //IN2
	0,     //IN3
	0,     //IN4
	ISC31, //IN5
	0,     //IN6
	0,     //IN7
	ISC21, //IN8
	ISC11, //IN9
};

static uint8_t INTn[9] = {
	0,    //IN1
	0,    //IN2
	0,    //IN3
	0,    //IN4
	INT3, //IN5
	0,    //IN6
	0,    //IN7
	INT2, //IN8
	INT1, //IN9
};

//PIN CHANGE INTERRUPTS
static volatile uint8_t * PCMSKn[9] = {
	&PCMSK2, //PD3 - IN1 PCBv1.0
//	&PCMSK1, //PC1 - IN1 PCBv1.1
	&PCMSK2, //PD2 - IN2
	&PCMSK2, //PD1 - IN3
	&PCMSK0, //PB7 - IN4
	&PCMSK1, //PC0 - IN5
	&PCMSK0, //PB6 - IN6
	&PCMSK2, //PD0 - IN7
	&PCMSK0, //PB5 - IN8
	&PCMSK0  //PB2 - IN9
};

static uint8_t PCINTn[9] = {
	PCINT19, //PD3 - IN1 PCBv1.0
//	PCINT9,  //PC1 - IN1 PCBv1.1
	PCINT18, //PD2 - IN2
	PCINT17, //PD1 - IN3
	PCINT7,  //PB7 - IN4
	PCINT8,  //PC0 - IN5
	PCINT6,  //PB6 - IN6
	PCINT16, //PD0 - IN7
	PCINT5,  //PB5 - IN8
	PCINT2   //PB2 - IN9
};

static uint8_t PCIEn[9] = {
	PCIE2, //PD3 PCBv1.0
//	PCIE1, //PC1 PCBv1.1
	PCIE2, //PD2
	PCIE2, //PD1
	PCIE0, //PB7
	PCIE1, //PC0
	PCIE0, //PB6
	PCIE2, //PD0
	PCIE0, //PB5
	PCIE0  //IN9
};

static volatile uint8_t pcint0_data = 0;
static volatile uint8_t pcint1_data = 0;
static volatile uint8_t pcint2_data = 0;

//EXTERNAL INTERRUPTS
uint8_t ext_int_on(uint8_t port, uint8_t isc1, uint8_t isc0) {
	if (port == IN5 || port == IN8 || port == IN9) {
		EICRA |= (isc1?1:0)<<ISCn1[port] | (isc0?1:0)<<ISCn0[port]; //set control bits
		EIMSK |= (1<<INTn[port]); //enable interrupt
		return 1;
	}
	return 0; //if not an ext. int. port, returns zero
}

uint8_t ext_int_off(uint8_t port) {
	if (port == IN5 || port == IN8 || port == IN9) {
		EICRA &= ~(1<<ISCn1[port] | 1<<ISCn0[port]); //clear control bits
		EIMSK &= ~(1<<INTn[port]); //disable interrupt
		return 1;
	}
	return 0; //if not an ext. int. port, returns zero
}

//PIN CHANGE INTERRUPTS
static void update_pcint_data(void) {
	pcint0_data = PINB & PCMSK0;
	pcint1_data = PINC & PCMSK0;
	pcint2_data = PIND & PCMSK2;
}

void pc_int_on(uint8_t port) {
	*PCMSKn[port] |= (1<<PCINTn[port]);
	PCICR |= (1<<PCIEn[port]);
	update_pcint_data();
}

void pc_int_off(uint8_t port) {
	*PCMSKn[port] &= ~(1<<PCINTn[port]);
	if(!(*PCMSKn[port])) {
		PCICR &= ~(1<<PCIEn[port]);
	}
	update_pcint_data();
}

//General
void interrupts_on() {
	update_pcint_data();
	sei();
}

void interrupts_off() {
	cli();
}

//INTERRUPT HANDLERS
ISR(PCINT0_vect) {
	uint8_t change_finder = (PINB & PCMSK0) ^ pcint0_data;
	update_pcint_data();

	if(change_finder & (1<<PCINTn[IN4])) {
		pcISR_in4();
	}
	if(change_finder & (1<<PCINTn[IN6])) {
		pcISR_in6();
	}
	if(change_finder & (1<<PCINTn[IN8])) {
		pcISR_in8();
	}
	if(change_finder & (1<<PCINTn[IN9])) {
		pcISR_in9();
	}
}

ISR(PCINT1_vect) {
	uint8_t change_finder = (PINC & PCMSK1) ^ pcint1_data;
	update_pcint_data();
	//PCBv1.1--
	if(change_finder & (1<<PCINTn[IN1])) {
		pcISR_in1();
	}
	//--PCBv1.1
	if(change_finder & (1<<PCINTn[IN5])) {
		pcISR_in5();
	}
}

ISR(PCINT2_vect) {
	uint8_t change_finder = (PIND & PCMSK2) ^ pcint2_data;
	update_pcint_data();
	//PCBv1.0--
	if(change_finder & (1<<PCINTn[IN1])) {
		pcISR_in1();
	}
	//--PCBv1.0
	if(change_finder & (1<<PCINTn[IN2])) {
		pcISR_in2();
	}
	if(change_finder & (1<<PCINTn[IN3])) {
		pcISR_in3();
	}
	if(change_finder & (1<<PCINTn[IN7])) {
		pcISR_in7();
	}
}
