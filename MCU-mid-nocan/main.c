/*
 * main.c - Entry Point for execution of code on the mid MCU used in the LUR7.
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
 * \ref main is the Entry Point for execution of code on the mid MCU used in
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
 * \defgroup main Main source file
 * \ref main.c is the top source code file for the mid LUR7 MCU.
 *
 * \todo failsafe / manual gear shift
 *
 * \see \ref main.c
 * \see \ref config
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "../header_and_config/LUR7.h"
#include "config.h"
#include "display.h"

//! The MOb configured for RX of logging start/stop instructions.
volatile uint8_t CAN_DTA_MOb;
//! Variable containing information on whether logging is active or not.
volatile uint8_t logging = FALSE;
//! Flag set when new information has been received and the panel is ready to be updated
volatile uint8_t new_info = TRUE;
//! Left clutch position sensor value.
volatile uint16_t clutch_pos_left = 0;
//! Right clutch position sensor value.
volatile uint16_t clutch_pos_right = 0;
//! Atomically written copy of left clutch position sensor value.
volatile uint16_t clutch_pos_left_atomic = 0;
//! Atomically written copy of right clutch position sensor value.
volatile uint16_t clutch_pos_right_atomic = 0;
//! Used for stopping clutch CAN messages.
volatile uint8_t clutch_CAN_disable = FALSE;
//! Debounce for gear shifting
volatile uint8_t gear_debounce = FALSE;
//! CAN unhang
volatile uint8_t dta_can_counter = 0;
volatile uint8_t rear_can_counter = 0;

volatile uint8_t gear_MOb = 0;

//! Main function.
/*!
 * The entry point of the execution of code for the middle MCU. All hardware that
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
	//! <ul>
	//! <li> SETUP <ul>
	//! <li> Initialisation <ol>
	io_init(); //! <li> initialise LUR_io.
	adc_init(); //! <li> initialise LUR7_adc.
	ancomp_init(); //! <li> initialise LUR7_ancomp.
	can_init(); //! <li> initialise LUR7_CAN.
	timer1_init(OFF); //! <li> initialise LUR7_timer1.
	timer0_init(); //! <li> initialise LUR7_timer0.
	//! </ol>

	//! <li> LUR7_power. <ol>
	power_off_default(); //! <li> power off unused periferals.
	//! </ol>

	//! <li> Setup CAN RX <ol>
	CAN_DTA_MOb = can_setup_rx(CAN_DTA_ID, CAN_DTA_MASK, CAN_DTA_DLC); //! <li> Reception of DTA packages, ID 0x2000-3.
	gear_MOb = can_setup_rx(0x12345, 0xffffffff, 1);
	//! </ol>
	
	pc_int_on(IN2);
	pc_int_on(IN3);
	pc_int_on(IN7);
	
	//! <li> Enable system <ol>
	interrupts_on(); //! <li> enable interrupts.
	can_enable(); //! <li> enable CAN.
	//! </ol>
	//! </ul>

	//! <li> LOOP <ul>
	while (1) {
		if (new_info) {
			new_info = FALSE; //! <li> clear flag
			update_display(get_current_gear()); //! <li> update panel
		} //! </ol>
	} //! </ul>
	return 0; //! </ul>
}

// Interrupts
//! Timer Interrupt, 100 Hz
/*!
 * In order to schedule tasks or perform them with a well defined time delta,
 * the 100 Hz interrupt generator of LUR7_timer0 is used.
 *
 * \note To ensure that no corrupted values are sent, only atomically written
 * copies of all variables are used.
 *
 * All tasks scheduled use the CAN bus to transmit information. To not have all
 * messages sent out simultaneously they are spread out across different
 * occurrences of the interrupt using \p interrupt_nbr to identify when to execute.
 * The following table shows how tasks are spread out.
 *
 * | \p interrupt_nbr | Clutch position |
 * | :--------------: | :-------------: |
 * | 0, 10, 20, .. 90 | x               |
 * | 1, 11, 21, .. 91 | x               |
 * | 2, 12, 22, .. 92 | x               |
 * | 3, 13, 23, .. 93 | x               |
 * | 4, 14, 24, .. 94 | x               |
 * | 5, 15, 25, .. 95 | x               |
 * | 6, 16, 26, .. 96 | x               |
 * | 7, 17, 27, .. 97 | x               |
 * | 8, 18, 28, .. 98 | x               |
 * | 9, 19, 29, .. 99 | x               |
 *
 * \param interrupt_nbr The id of the interrupt, counting from 0-99.
 */

void timer1_isr_100Hz(uint8_t interrupt_nbr) {
	
	if (interrupt_nbr % 20 == 0) {
		new_info = TRUE; //! <li> set flag to update panel
	}
	
	if (++dta_can_counter > 20 && dta_can_counter % 10 == 0) {
		can_free_rx(CAN_DTA_MOb);
		CAN_DTA_MOb = can_setup_rx(CAN_DTA_ID, CAN_DTA_MASK, CAN_DTA_DLC);
		
		if (CAN_DTA_MOb  != 0xff) {
			dta_can_counter = 0;
		}
		
		update_RPM(0);
	}
	
	if (++rear_can_counter > 20 && rear_can_counter % 10 == 0) {
		can_free_rx(gear_MOb);
		gear_MOb = can_setup_rx(0x12345, 0xffffffff, 1);
		
		if (gear_MOb  != 0xff) {
			rear_can_counter = 0;
		}

		update_gear(10);
	}
}

void timer0_isr_stop(void) {}

void pcISR_in1(void) {}
void pcISR_in2(void) {
	if (!get_input(IN2)) {
		can_setup_tx(CAN_GEAR_ID, CAN_MSG_POT_DISS, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	}
}

void pcISR_in3(void) {
	if (!get_input(IN3)) {
		can_setup_tx(CAN_GEAR_ID, CAN_MSG_GEAR_NEUTRAL_REPEAT, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	}
}

void pcISR_in4(void) {}
void pcISR_in5(void) {}
void pcISR_in6(void) {}
void pcISR_in7(void) {
	if (!get_input(IN7)) {
		can_setup_tx(CAN_GEAR_ID, CAN_MSG_POT_GOOD, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	}
}
void pcISR_in8(void) {}
void pcISR_in9(void) {}

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {
	//! <ul>
	if (mob == CAN_DTA_MOb) { //! <li> if received from DTA: <ul>
		dta_can_counter = 0;
		if (id == 0x2000) { //! <li> ID = 0x2000. <ul>
			update_RPM((data[6] << 8) | data[7]); //! <li> extract RPM.
			update_TPS((data[4] << 8) | data[5]); //! <li> extract TPS.
		}
	} //! </ul>
	
	else if (mob == gear_MOb) {
		rear_can_counter = 0;
		update_gear(data[0]);
	}
	//! </ul>
}

void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {
	uint8_t mob = (CANPAGE & 0xF0) >> 4; // get mob number
	if (mob == CAN_DTA_MOb) {
		CANCDMOB &= ~((1 << CONMOB1) | (1 << CONMOB0)); //disable MOb
		_NOP();
		CANCDMOB |= (1 << CONMOB1); // re-enable reception
	}
}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
