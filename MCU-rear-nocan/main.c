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
#include "gear_launch.h"
#include "clutch.h"
#include "brake.h"

//! Flag to set if signal to change up is received.
volatile uint8_t gear_up_flag = FALSE;
//! Flag to set if signal to change down is received.
volatile uint8_t gear_down_flag = FALSE;
//! Flag to set if signal to change to neutral (single attempt) is received.
volatile uint8_t gear_neutral_single_flag = FALSE;
//! Flag to set if signal to change to neutral (repeated attempts) is received.
volatile uint8_t gear_neutral_repeat_flag = FALSE;

//! flag for updating clutch
volatile uint16_t clutch_flag = FALSE;
//! atomic clutch readings
volatile uint16_t clutch_left_atomic = 0;
//! atomic clutch readings
volatile uint16_t clutch_right_atomic = 0;
//! voltage reading of gear pot, received from DTA
volatile uint16_t gear_pot_atomic = 0;

int main(void) {
	io_init();
	adc_init();
	ancomp_init();
	//can_init();
	timer0_init();
	timer1_init(ON);

	power_off_default();

	clutch_init();
	
	pc_int_on(BAK_IN_GEAR_UP); // gear up backup
	pc_int_on(BAK_IN_GEAR_DOWN); // gear down backup
	pc_int_on(BAK_IN_NEUTRAL); // gear neutral backup
	
	interrupts_on();

	while (1) {
		if (gear_up_flag) {
			gear_up();
			gear_up_flag = FALSE;
		}
		if (gear_down_flag) {
			gear_down();
			gear_down_flag = FALSE;
		}
		if (gear_neutral_single_flag) {
			gear_neutral_single();
			gear_neutral_single_flag = FALSE;
		}
		if (clutch_flag) {
			clutch_filter_left(clutch_left_atomic);
			clutch_filter_right(clutch_right_atomic);
			clutch_dutycycle_left();
			clutch_dutycycle_right();
			ATOMIC_BLOCK(ATOMIC_FORCEON) {
				clutch_set_dutycycle();
			}
			clutch_flag = FALSE;
		}

		uint16_t gear_pot = adc_get(IN8);
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			gear_pot_atomic = gear_pot;
		} // end ATOMIC_BLOCK

		//! <li> Brake control <ol>
		uint16_t brake = adc_get(BAK_IN_BRAKE); //! <li> update the brake pressure value.
		brake_light(brake); //! <li> set brake light on/off.
		//! </ol>

		//! <li> Clutch control <ol>
		uint16_t clutch = adc_get(BAK_IN_CLUTCH); //! <li> update clutch position value.
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			clutch_left_atomic = 0;
			clutch_right_atomic = clutch;
		} //! </ol>
	} //! </ul>
	//! </ul>
	return 0;
}

void pcISR_in1(void) {}
void pcISR_in2(void) {}
void pcISR_in3(void) {}
void pcISR_in4(void) {}
void pcISR_in5(void) {}
void pcISR_in6(void) {}
void pcISR_in7(void) {}
void pcISR_in8(void) {}
void pcISR_in9(void) {}

void timer1_isr_100Hz(uint8_t interrupt_nbr) {
	clutch_flag = TRUE;
	
	if (gear_pot_atomic > 71 && gear_pot_atomic <= 131){
		set_current_gear(1); // 449
	} else if (gear_pot_atomic > 131 && gear_pot_atomic <= 223){
		set_current_gear(0); // 930
	} else if (gear_pot_atomic > 223 && gear_pot_atomic <= 355){
		set_current_gear(2); // 1254
	} else if (gear_pot_atomic > 355 && gear_pot_atomic <= 553){
		set_current_gear(3); // 2216
	} else if (gear_pot_atomic > 553 && gear_pot_atomic <= 752){
		set_current_gear(4); // 3193
	} else if (gear_pot_atomic > 752 && gear_pot_atomic < 870){
		set_current_gear(5); // 4150
	}
	else {
		set_current_gear(POT_FAIL);
	}
	
	
}

//see gear_clutch.c
//void timer0_isr_stop(void) {}

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
