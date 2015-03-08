/*
 * / main.c - A collection of functions to setup and ease the use of the LUR7 PCB
 * / Copyright (C) 2015  Simon Wrafter <simon.wrafter@gmail.com>
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

/*! \file front-MCU/main.c
 * \ref front-main is the Entry Point for execution of code on the front MCU used in 
 * the \ref LUR7.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref front-MCU/config.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup front-main Front MCU - Main source file
 * \ref front-MCU/main.c is the top source code file for the front LUR7 MCU.
 *
 * \see \ref front-config
 * \see \ref LUR7
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "../header_and_config/LUR7.h"
#include "config.h"

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
//! The steering wheel angle sensor is stored here.
volatile uint16_t steering = 0;
//! An atomically written and safe copy of steering wheel sensor for use in interrupt.
volatile uint16_t steering_atomic = 0;

//! Brake pressure variable.
volatile uint16_t brake = 0;
//! An atomically written and safe copy of brake pressure sensor for use in interrupt.
volatile uint16_t brake_atomic = 0;
//! Status of the brake light on the rear-MCU. (assumed)
volatile uint8_t brake_light_status = OFF; // is there a way of actually knowing?

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
 * \ref front-main function.
 * 
 * The structure of main is:
 */
int main(void) {
	//! <ul> <li> Initialisation <ol>
	io_init(); //! <li> initialise \ref LUR7_io.
	adc_init(); //! <li> initialise \ref LUR7_adc.
	ancomp_init(); //! <li> initialise \ref LUR7_ancomp.
	can_init(); //! <li> initialise \ref LUR7_CAN.
	timer0_init(); //! <li> initialise \ref LUR7_timer0.
	//! </ol> <li> \ref LUR7_power. <ol>
	power_off_default(); //! <li> power off unused periferals.
	power_off_timer1(); //! <li> no PWM output is required, so \ref LUR7_timer1 is powered off. </ol>
	
	//! <li> Setup CAN RX <ol>
	log_MOb = can_setup_rx(CAN_LOG_ID, CAN_LOG_MASK, CAN_LOG_DLC); //! <li> Reception of logging instructions. </ol>
	
	//! <li> Eneable system <ol>
	interrupts_on(); //! <li> enable interrupts.
	can_enable(); //! <li> enable CAN. </ol>
	//! </ul>
	
	while (1) {
		brake = adc_get(BRAKE_PRESSURE);
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			brake_atomic = brake;
		} // end ATOMIC_BLOCK
		brake_light_get_new = FALSE;
		
		if (logging) {
			susp_l = adc_get(SUSPENSION_L);
			ATOMIC_BLOCK(ATOMIC_FORCEON) {
				susp_l_atomic = susp_l;
			} // end ATOMIC_BLOCK
			
			susp_r = adc_get(SUSPENSION_R);
			ATOMIC_BLOCK(ATOMIC_FORCEON) {
				susp_r_atomic = susp_r;
			} // end ATOMIC_BLOCK
			
			steering = adc_get(STEERING_WHEEL);
			ATOMIC_BLOCK(ATOMIC_FORCEON) {
				steering_atomic = steering;
			} // end ATOMIC_BLOCK
		}
	}
	return 0;
}

//WHEEL_R
/*!
 * b
 */
void pcISR_in1(void) {
	wheel_count_r++;
}
//WHEEL_L
/*!
 * b
 */
void pcISR_in2(void) {
	wheel_count_l++;
}
/*!
 * b
 */
void pcISR_in3(void) {}
/*!
 * b
 */
void pcISR_in4(void) {}
/*!
 * b
 */
void pcISR_in5(void) {}
/*!
 * b
 */
void pcISR_in6(void) {}
/*!
 * b
 */
void pcISR_in7(void) {}
/*!
 * b
 */
void pcISR_in8(void) {}
/*!
 * b
 */
void pcISR_in9(void) {}

//! H
void timer0_isr_100Hz(uint8_t interrupt_nbr) {
	if (brake_atomic >= BRAKES_ON && brake_light_status == OFF) {
		brake_light_status = ON;
		can_setup_tx(CAN_BRAKE_LIGHT_ID, (uint8_t *) &CAN_MSG_BRAKE_ON, CAN_FRONT_LOG_DLC);
	} else if (brake_atomic < BRAKES_ON && brake_light_status == ON) {
		brake_light_status = OFF;
		can_setup_tx(CAN_BRAKE_LIGHT_ID, (uint8_t *) &CAN_MSG_BRAKE_OFF, CAN_FRONT_LOG_DLC);
	}
	
	if (logging && !(interrupt_nbr % 10)) { //10 Hz
		uint32_t holder = ((uint32_t) wheel_count_l << 16) | wheel_count_r;
		can_setup_tx(CAN_FRONT_LOG_SPEED_ID, (uint8_t *) &holder, CAN_FRONT_LOG_DLC);
		wheel_count_l = 0;
		wheel_count_r = 0;
	}
	
	if (logging) {
		if (!((interrupt_nbr + 2) % 5)) { //20 Hz
			uint32_t holder = ((uint32_t) susp_l_atomic << 16) | susp_r_atomic;
			can_setup_tx(CAN_FRONT_LOG_SPEED_ID, (uint8_t *) &holder, CAN_FRONT_LOG_DLC);
		}
		
		if (!((interrupt_nbr + 4) % 5)) { //20 Hz
			uint32_t holder = ((uint32_t) brake_atomic << 16) | steering_atomic;
			can_setup_tx(CAN_FRONT_LOG_STEER_BRAKE_ID, (uint8_t *) &holder, CAN_FRONT_LOG_DLC);
		}
	}
}
//! H
void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {
	if (id == CAN_LOG_ID) {
		if (*data == CAN_MSG_LOG_START) {
			logging = TRUE;
			pc_int_on(WHEEL_L);
			pc_int_on(WHEEL_R);
		} else if (*data == CAN_MSG_LOG_STOP) {
			logging = FALSE;
			pc_int_off(WHEEL_L);
			pc_int_off(WHEEL_R);
		}
	}
}
//! H
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
//! H
void CAN_ISR_OTHER(void) {}

//! H
void early_bod_warning_ISR(void) {}
//! H
void early_bod_safe_ISR(void) {}
