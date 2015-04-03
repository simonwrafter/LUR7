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
#include "gear_clutch_launch.h"
#include "brake.h"

//! Flag to set if signal to change up is received.
volatile uint8_t gear_up_flag = FALSE;
//! Flag to set if signal to change down is received.
volatile uint8_t gear_down_flag = FALSE;
//! Flag to set if signal to change to neutral (single attempt) is received.
volatile uint8_t gear_neutral_single_flag = FALSE;
//! Flag to set if signal to change to neutral (repeated attempts) is received.
volatile uint8_t gear_neutral_repeat_flag = FALSE;

//! Whether the front MCU is in failsafe mode.
volatile uint8_t failsafe_front = FALSE;
//! Counter to put front MCU is in failsafe mode.
volatile uint8_t failsafe_front_counter = 0;
//! Whether the front MCU is in failsafe mode.
volatile uint8_t failsafe_mid = FALSE;
//! Counter to put front MCU is in failsafe mode.
volatile uint8_t failsafe_mid_counter = 0;
//! Whether the DTA is in failsafe mode.
volatile uint8_t failsafe_dta = FALSE;
//! Counter to put DTA is in failsafe mode.
volatile uint8_t failsafe_dta_counter = 0;

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

//! backup atomic clutch readings
volatile uint16_t atomic_clutch_pos = 0;

//! The MOb configured for RX of gear, clutch and launch control instructions.
volatile uint8_t gcl_MOb;
//! The MOb configured for RX of brake pressure.
volatile uint8_t brk_MOb;
//! The MOb configured for RX of current gear.
volatile uint8_t dta_MOb;

//! Main function.
/*!
 * The entry point of the execution of code for the rear MCU. All hardware that
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
	timer0_init(); //! <li> initialise LUR7_timer0.
	timer1_init(ON); //! <li> initialise LUR7_timer1.
	//! </ol>

	//! <li> LUR7_power. <ol>
	power_off_default(); //! <li> power off unused periferals.
	//! </ol>

	//! <li> Setup CAN RX <ol>
	gcl_MOb = can_setup_rx(CAN_GEAR_ID, CAN_GEAR_CLUTCH_LAUNCH_MASK, CAN_GEAR_CLUTCH_LAUNCH_DLC); //! <li> Reception of gear and clutch instructions.
	brk_MOb = can_setup_rx(CAN_FRONT_LOG_STEER_BRAKE_ID, CAN_FRONT_LOG_BRAKE_MASK, CAN_FRONT_LOG_DLC); //! <li> Reception of brake light instructions.
	dta_MOb = can_setup_rx(CAN_DTA_GEAR_ID, CAN_DTA_GEAR_MASK, CAN_DTA_DLC); //! <li> Reception of current gear from DTA.
	//! </ol>

	//! <li> Enable system <ol>
	set_output(GND_CONTROL, GND); //! <li> connect sensors to ground.
	interrupts_on(); //! <li> enable interrupts.
	can_enable(); //! <li> enable CAN.
	//! </ol>
	//! </ul>

	//! <li> LOOP <ul>
	while (1) {

		//! <li> Always do: <ol>
		if (gear_up_flag) { //! <li> if gear_up_flag is set. <ol>
			gear_up(); //! <li> change up a gear.
			gear_up_flag = FALSE;  //! <li> clear gear_up_flag.
		} //! </ol>
		if (gear_down_flag) { //! <li> if gear_down_flag is set. <ol>
			gear_down(); //! <li> change down a gear.
			gear_down_flag = FALSE; //! <li> clear gear_down_flag.
		} //! </ol>
		if (gear_neutral_single_flag) { //! <li> if neutral flag is set. <ol>
			gear_neutral_single(); //! <li> change to neutral gear. (single attempt)
			gear_neutral_single_flag = FALSE; //! <li> clear neutral flag.
		} //! </ol>
		if (gear_neutral_repeat_flag) { //! <li> if neutral flag is set. <ol>
			gear_neutral_repeat_linear(); //! <li> change to neutral gear. (repeat attempt)
			//gear_neutral_repeat_binary(); //! <li> change to neutral gear. (repeat attempt)
			gear_neutral_repeat_flag = FALSE; //! <li> clear neutral flag.
		} //! </ol>
		//! </ol>

		susp_l = adc_get(SUSPENSION_L); //! <li> update left suspension value.
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			susp_l_atomic = susp_l; //! <li> atomic copy of left suspension value.
		} // end ATOMIC_BLOCK

		susp_r = adc_get(SUSPENSION_R); //! <li>  update right suspension value.
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			susp_r_atomic = susp_r; //! <li> atomic copy of right suspension value.
		} // end ATOMIC_BLOCK

		//! <li> If front in failsafe, do: <ul>
		if (failsafe_front) {
			//! <li> Brake control <ol>
			uint16_t brake = adc_get(BAK_IN_BRAKE); //! <li> update the brake pressure value.
			brake_light(brake); //! <li> set brake light on/off.
			//! </ol>
		} //! </ul>

		//! <li> If mid in failsafe, do: <ul>
		if (failsafe_mid) {
			//! <li> Clutch control <ol>
			uint16_t clutch = adc_get(BAK_IN_CLUTCH); //! <li> update clutch position value.
			ATOMIC_BLOCK(ATOMIC_FORCEON) {
				atomic_clutch_pos = clutch;
			}
			//! </ol>
		} //! </ul>
		// dta failsafe not needed here, all sorted in timer1_isr_100Hz
	} //! </ul>
	//! </ul>
	return 0;
}

//! Pin Change Interrupt handler for IN1.
/*!
 * \ref WHEEL_R causes an interrupt incrementing the value of \ref wheel_count_r.
 */
void pcISR_in1(void) {
	wheel_count_r++;
}

//! Pin Change Interrupt handler for IN2.
/*!
 * \ref WHEEL_R causes an interrupt incrementing the value of \ref wheel_count_r.
 */
void pcISR_in2(void) {
	wheel_count_l++;
}

//! Pin Change Interrupt handler for IN3.
/*! Gear Up backup, enabled if mid-MCU is in failsafe mode. */
void pcISR_in3(void) {
	if (get_input(BAK_IN_GEAR_UP)) {
		gear_up_flag = TRUE;
	}
}

//! Pin Change Interrupt handler for IN4.
/*! \warning used as analog input */
void pcISR_in4(void) {}

//! Pin Change Interrupt handler for IN5.
/*! Gear Down backup, enabled if mid-MCU is in failsafe mode. */
void pcISR_in5(void) {
	if (get_input(BAK_IN_GEAR_DOWN)) {
		gear_down_flag = TRUE;
	}
}

//! Pin Change Interrupt handler for IN6.
/*! \warning used as analog input */
void pcISR_in6(void) {}

//! Pin Change Interrupt handler for IN7.
/*! Neutral Gear backup, enabled if mid-MCU is in failsafe mode. */
void pcISR_in7(void) {
	if (get_input(BAK_IN_NEUTRAL)) {
		gear_neutral_single_flag = TRUE;
	}
}

//! Pin Change Interrupt handler for IN8.
/*! \warning used as analog input */
void pcISR_in8(void) {}

//! Pin Change Interrupt handler for IN9.
/*! \warning used as analog input */
void pcISR_in9(void) {}

//! Timer Interrupt, 100 Hz
/*!
 * In order to schedule tasks or perform them with a well defined time delta,
 * the 100 Hz interrupt generator of LUR7_timer1 is used.
 *
 * \note To ensure that no corrupted values are sent, only atomically written
 * copies of all variables are used.
 *
 * All tasks scheduled use the CAN bus to transmit information. To not have all
 * messages sent out simultaneously they are spread out across different
 * occurrences of the interrupt using \p interrupt_nbr to identify when to execute.
 * The following table shows how tasks are spread out.
 *
 * | \p interrupt_nbr | Wheel speed log | suspension log |
 * | :--------------: | :-------------: | :------------: |
 * | 0, 10, 20, .. 90 | x               |                |
 * | 1, 11, 21, .. 91 |                 |                |
 * | 2, 12, 22, .. 92 |                 |                |
 * | 3, 13, 23, .. 93 |                 | x              |
 * | 4, 14, 24, .. 94 |                 |                |
 * | 5, 15, 25, .. 95 |                 |                |
 * | 6, 16, 26, .. 96 |                 |                |
 * | 7, 17, 27, .. 97 |                 |                |
 * | 8, 18, 28, .. 98 |                 | x              |
 * | 9, 19, 29, .. 99 |                 |                |
 *
 * \param interrupt_nbr The id of the interrupt, counting from 0-99.
 */
void timer1_isr_100Hz(uint8_t interrupt_nbr) {
	if (!failsafe_front && ++failsafe_front_counter == 100) {
		failsafe_front = TRUE;
		can_free_rx(brk_MOb);
	}

	if (!failsafe_mid && ++failsafe_mid_counter == 100) {
		failsafe_mid = TRUE;
		can_free_rx(gcl_MOb);
		pc_int_on(BAK_IN_GEAR_UP); // gear up backup
		pc_int_on(BAK_IN_GEAR_DOWN); // gear down backup
		pc_int_on(BAK_IN_NEUTRAL); // gear neutral backup
	}

	if (failsafe_mid) {
		clutch_set(atomic_clutch_pos);
	}

	if (!failsafe_dta && ++failsafe_dta_counter == 100) {
		failsafe_dta = TRUE;
		can_free_rx(dta_MOb);
		set_current_gear(11);
	}

	// 10 Hz (avoid other data being sent)
	if ((interrupt_nbr % 10) == 0) {
		uint32_t holder = ((uint32_t) wheel_count_l << 16) | wheel_count_r; // build data to send
		can_setup_tx(CAN_REAR_LOG_SPEED_ID, (uint8_t *) &holder, CAN_REAR_LOG_DLC); // send
		wheel_count_l = 0; // reset
		wheel_count_r = 0; // reset
	}

	//20 Hz (avoid other data being sent)
	if (((interrupt_nbr + 2) % 5) == 0) {
		uint32_t holder = ((uint32_t) susp_l_atomic << 16) | susp_r_atomic; // build data
		can_setup_tx(CAN_REAR_LOG_SUSPENSION_ID, (uint8_t *) &holder, CAN_REAR_LOG_DLC); // send
	}
}

//see gear_clutch.c
//void timer0_isr_stop(void) {}

//! CAN message receiver function.
/*!
 * The Rear MCU listens to messages from the middle and front MCUs, the
 * information extracted from these packages controls the brake light and gear
 * changing / clutch.
 *
 * The messages are read as follows:
 */
void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {
	//! <ul>
	if (mob == gcl_MOb) { //! <li> \ref gc_MOb receives a message <ul>
		failsafe_mid_counter = 0; //! <li> reset \ref failsafe_mid_counter
		if (id == CAN_GEAR_ID) { //! <li> gear change message <ul>
			uint16_t gear_data = ((uint16_t) data[1] << 8) | data[0];
			if (gear_data == CAN_MSG_GEAR_UP) { //! <li> if message is CAN_MSG_GEAR_UP, set \ref gear_up_flag to TRUE.
				gear_up_flag = TRUE;
			} else if (gear_data == CAN_MSG_GEAR_DOWN) { //! <li> if message is CAN_MSG_GEAR_DOWN, set \ref gear_down_flag to TRUE.
				gear_down_flag = TRUE;
			} else if (gear_data == CAN_MSG_GEAR_NEUTRAL_SINGLE) { //! <li> if msg is CAN_MSG_GEAR_NEUTRAL_SINGLE, set neutral_s to TRUE.
				gear_neutral_single_flag = TRUE;
			} else if (gear_data == CAN_MSG_GEAR_NEUTRAL_REPEAT) { //! <li> if msg is CAN_MSG_GEAR_NEUTRAL_REAPEAT, set neutral_r to TRUE.
				gear_neutral_repeat_flag = TRUE;
			}
		} else if (id == CAN_CLUTCH_ID) { //! <li> if message ID is CAN_CLUTCH_ID <ul>
			uint16_t clutch_p = ((uint16_t) data[1] << 8) | data[0]; //! <li> reconstruct clutch position
			clutch_set(clutch_p); //! <li> set clutch pwm.
		} else if (id == CAN_LAUNCH_ID) { //! <li> if message ID is CAN_LAUNCH_ID <ul>
			launch_control(); //! engage launch control.
		} //! </ul>
	} //! </ul>

	if (mob == brk_MOb) { //! <li> \ref brk_MOb receives a message <ul>
		failsafe_front_counter = 0; //! <li> reset \ref failsafe_front_counter
		uint16_t brake_p = ((uint16_t) data[3] << 8) | data[2]; //! <li> reconstruct brake pressure
		brake_light(brake_p); //! <li> control brake light
	} //! </ul>

	if (mob == dta_MOb) { //! <li> \ref dta_MOb receives a message <ul>
		failsafe_dta_counter = 0; //! <li> reset \ref failsafe_front_counter
		set_current_gear(data[0]); //! <li> relay to gear system.
	} //! </ul>

//! </ul>
}

//! CAN TX completion.
/*!
 * When transmission is complete this function is executed.
 */
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
//! CAN Error handler.
/*!
 * \todo implement CAN error handling
 */
void CAN_ISR_OTHER(void) {}

//! Brown Out warning.
/*!
 * \todo implement BOD handling
 */
void early_bod_warning_ISR(void) {}
//! Brown Out warning over.
/*!
 * \todo implement BOD handling
 */
void early_bod_safe_ISR(void) {}
