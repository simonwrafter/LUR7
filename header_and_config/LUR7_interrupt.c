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

/*! \file LUR7_interrupt.c
 * \ref LUR7_interrupt provides functions for setting up and using input
 * interrupts on the LUR7 PCB. 
 * 
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 *
 * \see LUR7_interrupt
 * \see LUR7_interrupt.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 * 
 * \defgroup LUR7_interrupt Shared - Input Interrupts
 * Input interrupts are a very powerful way of controling surrounding systems
 * without having to constantly check their status. Two kinds of interrupts are
 * available on the ATmega32M1, external and pin-change interrupts. \ref IN5,
 * \ref IN8 and \ref IN9 are external interrupts, where as all eight inputs
 * can function as pin-change interrupts. For a more detailed description of how
 * interrupts function on the ATmega32M1, see chapters 11 and 12 of the
 * ATmega32M1 datasheet. How too use interrupts with the LUR7 PCB is described
 * more closelly in \ref LUR7_interrupt.c.
 * 
 * \see LUR7_interrupt.c
 * \see LUR7_interrupt.h
 * \see <http://www.atmel.com/devices/ATMEGA32M1.aspx>
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "LUR7.h"
#include "LUR7_interrupt.h"

/*
 * static function declarations
 */
static void _update_pcint_data(void);


//EXTERNAL INTERRUPTS
//! Interrupt Sense Control bit 0
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

//! Interrupt Sense Control bit 1
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

//! External Interrupt Mask bits
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
//! Pin Change Mask Registers
static volatile uint8_t * PCMSKn[9] = {
	&PCMSK2, //PD3 - IN1
	&PCMSK2, //PD2 - IN2
	&PCMSK2, //PD1 - IN3
	&PCMSK0, //PB7 - IN4
	&PCMSK1, //PC0 - IN5
	&PCMSK0, //PB6 - IN6
	&PCMSK2, //PD0 - IN7
	&PCMSK0, //PB5 - IN8
	&PCMSK0  //PB2 - IN9
};

//! Pin Change Enable Mask bit
static uint8_t PCINTn[9] = {
	PCINT19, //PD3 - IN1
	PCINT18, //PD2 - IN2
	PCINT17, //PD1 - IN3
	PCINT7,  //PB7 - IN4
	PCINT8,  //PC0 - IN5
	PCINT6,  //PB6 - IN6
	PCINT16, //PD0 - IN7
	PCINT5,  //PB5 - IN8
	PCINT2   //PB2 - IN9
};

//! Pin Change Interrupt Enable bit
static uint8_t PCIEn[9] = {
	PCIE2, //PD3
	PCIE2, //PD2
	PCIE2, //PD1
	PCIE0, //PB7
	PCIE1, //PC0
	PCIE0, //PB6
	PCIE2, //PD0
	PCIE0, //PB5
	PCIE0  //IN9
};

//! Copy of the input as it was last time a Pin Change interrupt happened for PCINT0.
static volatile uint8_t pcint0_data = 0;
// only one interrupt on pcint1, no help variable needed.
//! Copy of the input as it was last time a Pin Change interrupt happened for PCINT2.
static volatile uint8_t pcint2_data = 0;

//EXTERNAL INTERRUPTS
//! Activates External Interrupt on \p port.
/*!
 * Valid ports for external interrupts are \ref IN5, \ref IN8 and \ref IN9. 
 * 
 * Quoting the ATmega32 manual the behaviour of external interrupts can be 
 * controlled through the settings of isc1 and isc0 as shown in the table below.
 * 
 * isc1  |  isc0  |  Interrupt behaviour
 * :---: | :----: | :--------------------------------------------------------------------------
 * 0     | 0      | The low level of INn generates an interrupt request.
 * 0     | 1      | Any logical change on INn generates an interrupt request.
 * 1     | 0      | The falling edge between two samples of INn generates an interrupt request.
 * 1     | 1      | The rising edge between two samples of INn generates an interrupt request.
 * 
 * To handle the generated interrupts the respective Interrupt Service Routines
 * need to be implemented. These are implemented as so:
 * \code{.c}
 * ISR(INT_INn_vect) {
 *     ; // do stuff
 * }
 * \endcode
 * Where the n is swappped out for the input number used. See \ref INT_IN5_vect,
 * \ref INT_IN8_vect and \ref INT_IN9_vect for more information.
 * 
 * \param port The input to activate External Interrupt for.
 * \param isc1 Controls the behaviour of the interrupt.
 * \param isc0 Controls the behaviour of the interrupt.
 * \return \ref TRUE if interrupt was successfully activated, \n \ref FALSE otherwise.
 */
uint8_t ext_int_on(uint8_t port, uint8_t isc1, uint8_t isc0) {
	if (port == IN5 || port == IN8 || port == IN9) { // check that port is external interrupt
		EICRA |= (isc1?1:0)<<ISCn1[port] | (isc0?1:0)<<ISCn0[port]; //set control bits
		EIMSK |= (1<<INTn[port]); //enable interrupt
		return TRUE; // return positive status
	}
	return FALSE; //if not an ext. int. port, returns zero
}

//! De-activates External Interrupt on \p port.
/*!
 * Turns External Interrupts off for the input \p port.
 * 
 * Valid ports for External Interrupts are \ref IN5, \ref IN8 and \ref IN9. 
 * 
 * \param port The input to de-activate External Interrupt for.
 * \return \ref TRUE if interrupt was successfully de-activated, \n \ref FALSE otherwise.
 */
uint8_t ext_int_off(uint8_t port) {
	if (port == IN5 || port == IN8 || port == IN9) { // check that port is external interrupt
		EICRA &= ~(1<<ISCn1[port] | 1<<ISCn0[port]); //clear control bits
		EIMSK &= ~(1<<INTn[port]); //disable interrupt
		return TRUE; // return positive status
	}
	return FALSE; //if not an ext. int. port, returns zero
}

//PIN CHANGE INTERRUPTS

//! Helper function, takes copies of inputs.
/*!
 * To figure out which Pin Change Interrupt just happened copies of the inputs
 * are kept. This function updates these copies by masking out the current input
 * values for those pins where pin change interrupts are active.
 */
static void _update_pcint_data() {
	pcint0_data = PINB & PCMSK0; // mask out input values for those pins where pin change interrupts are active
	pcint2_data = PIND & PCMSK2; // mask out input values for those pins where pin change interrupts are active
}

//! Activates Pin Change Interrupt on \p port.
/*!
 * Turns Pin Change Interrupt on for \p port. When a interrupt is generated
 * the corresponding extern function is executed. These functions need to be
 * defined in the main project as:
 * \code{.c}
 * void pcISR_inX(void) {
 *     ; // do stuff
 * }
 * \endcode
 * Where inX is replaced with in1-in9 for the respective inputs.
 * 
 * Interrupts are generated on logical value changes to the input. Unlike External Interrupts this behaviour can not be altered.
 * 
 * \param port The input to activate Pin Change Interrupt for. Valid ports for
 * Pin Change Interrupts are \ref IN1 - \ref IN9.
 * \return \ref TRUE if interrupt was successfully activated, \n \ref FALSE otherwise.
 */
uint8_t pc_int_on(uint8_t port) {
	if (port >= FIRST_IN && port <= LAST_IN) {
		*PCMSKn[port] |= (1<<PCINTn[port]);
		PCICR |= (1<<PCIEn[port]);
		_update_pcint_data();
		return TRUE;
	}
	return FALSE;
}

//! De-activates Pin Change Interrupt on \p port.
/*!
 * Turns Pin Change Interrupt off for the input \p port.
 * 
 * \param port The input to de-activate Pin Change Interrupt for. Valid ports for
 * Pin Change Interrupts are \ref IN1 - \ref IN9.
 * \return \ref TRUE if interrupt was successfully de-activated, \n \ref FALSE otherwise.
 */
uint8_t pc_int_off(uint8_t port) {
	if (port >= FIRST_IN && port <= LAST_IN) {
		*PCMSKn[port] &= ~(1<<PCINTn[port]);
		if(!(*PCMSKn[port])) {
			PCICR &= ~(1<<PCIEn[port]);
		}
		_update_pcint_data();
		return TRUE;
	}
	return FALSE;
}

//General
//! Activate interrupts globaly
/*!
 * Sets the global interrupt enable bit in SREG and updates the information
 * about Pin Change Interrupts.
 */
void interrupts_on(void) {
	_update_pcint_data();
	sei();
}

//! De-activate interrupts globaly
/*!
 * Clears the global interrupt enable bit in SREG.
 */
void interrupts_off(void) {
	cli();
}

//INTERRUPT HANDLERS
//! Interrupt Service Routine, Pin Change Interrupt 0
/*!
 * Upon a Pin Change Interrupt the ISR checks which input changed and executes
 * the correct function.
 * 
 * \see pcISR_in4
 * \see pcISR_in6
 * \see pcISR_in8
 * \see pcISR_in9
 */
ISR(PCINT0_vect) {
	uint8_t change_finder = (PINB & PCMSK0) ^ pcint0_data;
	_update_pcint_data();

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

//! Interrupt Service Routine, Pin Change Interrupt 1
/*!
 * There is only one input connected to this flag so it is assumed that IN5 
 * changed should the flag be set.
 * 
 * \see pcISR_in5
 */
ISR(PCINT1_vect) {
	pcISR_in5();
}

//! Interrupt Service Routine, Pin Change Interrupt 2
/*!
 * Upon a Pin Change Interrupt the ISR checks which input changed and executes
 * the correct function.
 * 
 * \see pcISR_in1
 * \see pcISR_in2
 * \see pcISR_in3
 * \see pcISR_in7
 */
ISR(PCINT2_vect) {
	uint8_t change_finder = (PIND & PCMSK2) ^ pcint2_data;
	_update_pcint_data();
	if(change_finder & (1<<PCINTn[IN1])) {
		pcISR_in1();
	}
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
