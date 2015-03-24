/*
 * main.c - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file main.c
 * \ref main is the Entry Point for execution of code on the MCU used in 
 * the LUR7.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref config.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup main Example Main source file
 * \ref main.c is the top source code file for the XXXX LUR7 MCU.
 *
 * \see \ref config
 * \see \ref LUR7
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "../header_and_config/LUR7.h"
#include "config.h"

//! Variable containing information on whether logging is active or not.
volatile uint8_t logging = FALSE;

//! Main function.
/*!
 * The entry point of the execution of code for the MCU. All hardware that
 * is to be used is configured and initialised and the remaining peripherals
 * powered off.
 * 
 * To the largest extent possible tasks are handled through interrupts. When
 * this is not possible the task may be performed in the loop of the 
 * \ref main function.
 * 
 * The structure of main is:
 */
int main(void) {
	//! <ul> <li> SETUP
	//! <ul> <li> Initialisation <ol>
	io_init(); //! <li> initialise \ref LUR7_io.
	adc_init(); //! <li> initialise \ref LUR7_adc.
	ancomp_init(); //! <li> initialise \ref LUR7_ancomp.
	can_init(); //! <li> initialise \ref LUR7_CAN.
	//timer0_init(); //! <li> initialise \ref LUR7_timer0.
	timer1_init(OFF); //! <li> initialise \ref LUR7_timer1.
	//! </ol> 
	//! <li> \ref LUR7_power. <ol>
	power_off_default(); //! <li> power off unused periferals.
	power_off_timer0(); //! <li> \ref LUR7_timer0 is powered off. 
	//! </ol>
	
	//! <li> Setup CAN RX <ol>
	; //! <li> nothing yet.
	//! </ol>
	
	/*
	 * Space for more setup code
	 * This code will be run once.
	 */
	
	//! <li> Eneable system <ol>
	interrupts_on(); //! <li> enable interrupts.
	can_enable(); //! <li> enable CAN.
	//! </ol>
	//! </ul>
	
	//! <li> LOOP
	while (1) {
		
		/*
		 * Space for main code
		 * This code will be run repeatedly.
		 */
		
		//! <ul> <li> Always do: <ol>
		; //! <li> nothing yet.
		//! </ol>
	} //! </ul>
	//! </ul>
	return 0;
}

/*! \note unused input */
void pcISR_in1(void) {}
/*! \note unused input */
void pcISR_in2(void) {}
/*! \note unused input */
void pcISR_in3(void) {}
/*! \note unused input */
void pcISR_in4(void) {}
/*! \note unused input */
void pcISR_in5(void) {}
/*! \note unused input */
void pcISR_in6(void) {}
/*! \note unused input */
void pcISR_in7(void) {}
/*! \note unused input */
void pcISR_in8(void) {}
/*! \note unused input */
void pcISR_in9(void) {}

/*!
 * In order to schedule tasks or perform them with a well defined time delta,
 * the 100 Hz interrupt generator of LUR7_timer0 is used.
 * 
 * \note To ensure that no corrupted values are sent during logging, only the
 * atomically written copies of all variables are used.
 * 
 * All tasks scheduled use the CAN bus to transmit information. To not have all 
 * messages sent out simultaneously they are spread out across different 
 * occurrences of the interrupt using \p interrupt_nbr to identify when to execute.
 * The following table shows how tasks are spread out.
 *
 * | \p interrupt_nbr | title 1 | title 2 |
 * | :--------------: | :-----: | :-----: |
 * | 0, 10, 20, .. 90 |         |         |
 * | 1, 11, 21, .. 91 |         |         |
 * | 2, 12, 22, .. 92 |         |         |
 * | 3, 13, 23, .. 93 |         |         |
 * | 4, 14, 24, .. 94 |         |         |
 * | 5, 15, 25, .. 95 |         |         |
 * | 6, 16, 26, .. 96 |         |         |
 * | 7, 17, 27, .. 97 |         |         |
 * | 8, 18, 28, .. 98 |         |         |
 * | 9, 19, 29, .. 99 |         |         |
 * 
 * \param interrupt_nbr The id of the interrupt, counting from 0-99.
 */
void timer1_isr_100Hz(uint8_t interrupt_nbr) {}
/*!
 * Executed when timer0 delay end.
 */
void timer0_isr_stop(void) {}
/*!
 * CAN messages received are handled here.
 */
void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
/*!
 * When transmission is complete this function is executed.
 */
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
/*!
 * \todo implement CAN error handling
 */
void CAN_ISR_OTHER(void) {}

/*!
 * \todo implement BOD handling
 */
void early_bod_warning_ISR(void) {}
/*!
 * \todo implement BOD handling
 */
void early_bod_safe_ISR(void) {}
