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
 * \ref main is the Entry Point for execution of code on the rear MCU used in 
 * the LUR7.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref main
 * \see \ref config.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup main Rear MCU - Main source file
 * \ref main.c is the top source code file for the rear LUR7 MCU.
 *
 * \see \ref main.c
 * \see \ref config
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "../header_and_config/LUR7.h"
#include "config.h"

//! Whether the system is in failsafe CAN mode.
volatile uint8_t failsafe = FALSE;

//! Variable containing information on whether logging is active or not.
volatile uint8_t logging = FALSE;

//! Counter for pulses from left wheel speed sensor.
volatile uint16_t wheel_count_l = 0;
//! Counter for pulses from right wheel speed sensor.
volatile uint16_t wheel_count_r = 0;
//! Suspension sensor position for left wheel.
volatile uint16_t susp_l = 0;
//! Suspension sensor position for right wheel.
volatile uint16_t susp_r = 0;
//! An atomically written and safe copy of suspension sensor for use in interrupt.
volatile uint16_t susp_l_atomic = 0;
//! An atomically written and safe copy of suspension sensor for use in interrupt.
volatile uint16_t susp_r_atomic = 0;

//! Brake pressure variable.
volatile uint16_t brake = 0;
//! An atomically written and safe copy of brake pressure sensor for use in interrupt.
volatile uint16_t brake_atomic = 0;

//! The MOb configured for RX of logging start/stop instructions.
volatile uint8_t log_MOb;

//! Main function.
/*!
 * The entry point of the execution of code for the front MCU. All hardware that
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
	io_init(); //! <li> initialise LUR_io.
	adc_init(); //! <li> initialise LUR7_adc.
	ancomp_init(); //! <li> initialise LUR7_ancomp.
	can_init(); //! <li> initialise LUR7_CAN.
	timer0_init(); //! <li> initialise LUR7_timer0.
	timer1_init(); //! <li> initialise LUR7_timer1.
	//! </ol> <li> LUR7_power. <ol>
	power_off_default(); //! <li> power off unused periferals.
	//power_off_timer1(); //! <li> no PWM output is required, so LUR7_timer1 is powered off. </ol>
	
	//! <li> Setup CAN RX <ol>
	log_MOb = can_setup_rx(CAN_LOG_ID, CAN_LOG_MASK, CAN_LOG_DLC); //! <li> Reception of logging instructions. </ol>
	
	//! <li> Eneable system <ol>
	interrupts_on(); //! <li> enable interrupts.
	can_enable(); //! <li> enable CAN. </ol>
	//! </ul>
	
	//! <li> LOOP
	while (1) {
		//! <ul> <li> Always do: <ol>
		; //! <li> 
		//! </ol>
		//! <li> If logging, do: <ol>
		if (logging) { 
			susp_l = adc_get(SUSPENSION_L); //! <li> update left suspension value.
			ATOMIC_BLOCK(ATOMIC_FORCEON) {
				susp_l_atomic = susp_l; //! <li> atomic copy of left suspension value.
			} // end ATOMIC_BLOCK
			
			susp_r = adc_get(SUSPENSION_R); //! <li>  update right suspension value.
			ATOMIC_BLOCK(ATOMIC_FORCEON) {
				susp_r_atomic = susp_r; //! <li> atomic copy of right suspension value.
			} // end ATOMIC_BLOCK
		} //! </ol>
		
		//! <li> If failsafe, do: <ol>
		if (failsafe) {
			brake = adc_get(BRAKE_PRESSURE); //! <li> update the brake pressure value.
			if (brake >= BRAKES_ON) { // check if brakes are being applied
				set_output(BRAKE_LIGHT, GND); // assume light will be lit
			} else if (brake_atomic < BRAKES_OFF) { // check if brakes are not being applied
				set_output(BRAKE_LIGHT, TRI); // set last message to be sent
			}
		} //! </ol>
		
		
	} //! </ul>
	
	//! </ul>
	return 0;
}

/*!
 * \ref WHEEL_R causes an interrupt incrementing the value of \ref wheel_count_r.
 */
void pcISR_in1(void) {
	wheel_count_r++;
}
/*!
 * \ref WHEEL_R causes an interrupt incrementing the value of \ref wheel_count_r.
 */
void pcISR_in2(void) {
	wheel_count_l++;
}
/*!
 * Gear Up backup 
 */
void pcISR_in3(void) {
	;
}
/*! \warning used as analog input */
void pcISR_in4(void) {}
/*!
 * Gear Down backup
 */
void pcISR_in5(void) {
	;
}
/*! \warning used as analog input */
void pcISR_in6(void) {}
/*!
 * Neutral Gear backup
 */
void pcISR_in7(void) {
	;
}
/*! \warning used as analog input */
void pcISR_in8(void) {}
/*! \warning used as analog input */
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
 * | \p interrupt_nbr | Wheel speed log | suspension log |
 * | :--------------: | :-------------: | :------------: |
 * | 0, 10, 20, .. 90 | (x) if logging  |                |
 * | 1, 11, 21, .. 91 |                 |                |
 * | 2, 12, 22, .. 92 |                 |                |
 * | 3, 13, 23, .. 93 |                 | (x) if logging |
 * | 4, 14, 24, .. 94 |                 |                |
 * | 5, 15, 25, .. 95 |                 |                |
 * | 6, 16, 26, .. 96 |                 |                |
 * | 7, 17, 27, .. 97 |                 |                |
 * | 8, 18, 28, .. 98 |                 | (x) if logging |
 * | 9, 19, 29, .. 99 |                 |                |
 * 
 * Packages containing logging data are only sent if logging is active.
 * 
 * \param interrupt_nbr The id of the interrupt, counting from 0-99.
 */
void timer0_isr_100Hz(uint8_t interrupt_nbr) {
	
	// if logging is active and 10Hz interrupt
	if (logging && !(interrupt_nbr % 10)) { //10 Hz
		uint32_t holder = ((uint32_t) wheel_count_l << 16) | wheel_count_r; // build data to send
		can_setup_tx(CAN_FRONT_LOG_SPEED_ID, (uint8_t *) &holder, CAN_FRONT_LOG_DLC); // send
		wheel_count_l = 0; // reset
		wheel_count_r = 0; // reset
	}
	
	
	if (logging) {
		if (!((interrupt_nbr + 2) % 5)) { //20 Hz (avoid other data being sent)
			uint32_t holder = ((uint32_t) susp_l_atomic << 16) | susp_r_atomic; // build data
			can_setup_tx(CAN_REAR_LOG_SPEED_ID, (uint8_t *) &holder, CAN_REAR_LOG_DLC); // send
		}
	}
}

/*!
 * CAN messages received are handled here. The front MCU is only interested in 
 * one message, whether to start or stop logging. When a message is received one 
 * of two things can happen depending on the contents of the data field.
 * 
 * Logging of data is then handled by \ref main and \ref timer0_isr_100Hz.
 */
void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {
	if (id == CAN_LOG_ID) { //! CAN_LOG_ID sends message
		if (*data == CAN_MSG_LOG_START) { //! <ul> <li> message is CAN_MSG_LOG_START
			logging = TRUE; //! <ol> <li> set \ref logging to TRUE.
			pc_int_on(WHEEL_L); //! <li> enable interrupts on \ref WHEEL_L.
			pc_int_on(WHEEL_R); //! <li> enable interrupts on \ref WHEEL_R. </ol>
		} else if (*data == CAN_MSG_LOG_STOP) { //! <li> message is CAN_MSG_LOG_STOP
			logging = FALSE; //! <ol> <li> set \ref logging to FALSE.
			pc_int_off(WHEEL_L); //! <li> disable interrupts on \ref WHEEL_L
			pc_int_off(WHEEL_R); //! <li> disable interrupts on \ref WHEEL_R. </ol>
		} //! </ul>
	}
}
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
