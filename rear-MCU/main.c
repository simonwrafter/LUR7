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
#include "gear_clutch.h"

//! Flag to set if signal to change up is received
volatile uint8_t gear_up_flag = FALSE;
//! Flag to set if signal to change down is received
volatile uint8_t gear_down_flag = FALSE;
//! Flag to set if signal to change to neutral is received
volatile uint8_t gear_neutral_flag = FALSE;

//! Whether the system is in failsafe mode.
volatile uint8_t failsafe = FALSE;
//! Counter to put system is in failsafe mode.
volatile uint8_t failsafe_counter = 0;

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

//! The MOb configured for RX of gear and clutch instructions.
volatile uint8_t gc_MOb;
//! The MOb configured for RX of brake pressure.
volatile uint8_t brk_MOb;

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
	timer1_init(); //! <li> initialise LUR7_timer1.
	//! </ol>
	//! <li> LUR7_power. <ol>
	power_off_default(); //! <li> power off unused periferals.
	//power_off_timer1(); //! <li> no PWM output is required, so LUR7_timer1 is powered off.
	//! </ol>

	//! <li> Setup CAN RX <ol>
	gc_MOb = can_setup_rx(CAN_GEAR_ID, CAN_GEAR_CLUTCH_MASK, CAN_GEAR_CLUTCH_DLC); //! <li> Reception of gear and clutch instructions.
	brk_MOb = can_setup_rx(CAN_FRONT_LOG_STEER_BRAKE_ID, CAN_FRONT_LOG_BRAKE_MASK, CAN_FRONT_LOG_DLC); //! <li> Reception of brake light instructions.
	//! </ol>

	//! <li> Enable system <ol>
	set_output(GROUND_CONTROL, GND); //! <li> connect sensors to ground.
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
		if (gear_neutral_flag) { //! <li> if gear_down_flag is set. <ol>
			gear_neutral(gear_neutral_flag); //! <li> change down a gear.
			gear_neutral_flag = FALSE; //! <li> clear gear_down_flag.
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

		//! <li> If failsafe, do: <ul>
		if (failsafe) {
			//! <li> Brake control <ol>
			uint16_t brake = adc_get(BAK_IN_BRAKE); //! <li> update the brake pressure value.
			if (brake > BRAKES_ON) { //! <li>  check if brakes are being applied
				set_output(BRAKE_LIGHT, GND); //! <li>  turn light on
			} else if (brake < BRAKES_OFF) { //! <li>  check if brakes are not being applied
				set_output(BRAKE_LIGHT, TRI); //! <li> turn light off
			} //! </ol>
			//! <li> Clutch control <ol>
			uint16_t clutch = adc_get(BAK_IN_CLUTCH); //! <li> update clutch position value.
			clutch_set(clutch);//! <li> set clutch pwm.
			//! </ol>
		} //! </ul>
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
/*! Gear Up backup */
void pcISR_in3(void) {
	gear_up_flag = TRUE;
}
//! Pin Change Interrupt handler for IN4.
/*! \warning used as analog input */
void pcISR_in4(void) {}
//! Pin Change Interrupt handler for IN5.
/*! Gear Down backup */
void pcISR_in5(void) {
	gear_down_flag = TRUE;
}
//! Pin Change Interrupt handler for IN6.
/*! \warning used as analog input */
void pcISR_in6(void) {}
//! Pin Change Interrupt handler for IN7.
/*! Neutral Gear backup */
void pcISR_in7(void) {
	gear_neutral_flag = TRUE;
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
void timer0_isr_100Hz(uint8_t interrupt_nbr) {
	if (++failsafe_counter == 50) {
		failsafe = TRUE;
		logging = FALSE;
		can_free_rx(brk_MOb);
		can_free_rx(gc_MOb);
		can_disable();
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
		can_setup_tx(CAN_REAR_LOG_SPEED_ID, (uint8_t *) &holder, CAN_REAR_LOG_DLC); // send
	}
}

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

	if (mob == gc_MOb) { //! <li> \ref gc_MOb receives a message <ul>
		if (id == CAN_GEAR_ID) { //! <li> gear change message <ul>
			if (*data == CAN_MSG_GEAR_UP) { //! <li> if message is CAN_MSG_GEAR_UP, set \ref gear_up_flag to TRUE.
				gear_up_flag = TRUE;
			} else if (*data == CAN_MSG_GEAR_DOWN) { //! <li> if message is CAN_MSG_GEAR_DOWN, set \ref gear_down_flag to TRUE.
				gear_down_flag = TRUE;
			} else if(*data & 0xFF00 == CAN_MSG_GEAR_NEUTRAL) { //! <li> if message is CAN_MSG_GEAR_NEUTRAL, set \ref gear_down_flag to TRUE.
				gear_neutral_flag = data[1];
			} //! </ul>
		}
		if (id == CAN_CLUTCH_ID) { //! <li> if message ID is CAN_CLUTCH_ID <ul>
			clutch_set(*data); //! <li> set clutch pwm.
			failsafe_counter = 0; //! <li> reset \ref failsafe_counter
		} //! </ul>
	} //! </ul>

	if (mob == brk_MOb) { //! <li> \ref brk_MOb receives a message <ul>
		uint16_t brake_p = ((uint16_t) data[2] << 8) | data[3];
		if (brake_p > BRAKE_ON) {
			set_output(BRAKE_LIGHT, GND);
		} else if (brake_p < BRAKE_OFF) {
			set_output(BRAKE_LIGHT, TRI);
		}
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
