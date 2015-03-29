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

#include "../header_and_config/LUR7.h"
#include "config.h"
#include "gear_clutch.h"
#include "brake.h"

// Flag to set if signal to change up is received.
volatile uint8_t gear_up_flag = FALSE;
// Flag to set if signal to change down is received.
volatile uint8_t gear_down_flag = FALSE;
// Flag to set if signal to change to neutral is received.
volatile uint8_t gear_neutral_flag = FALSE;
// Variable holding the current gear as perceived by the DTA S60pro.
volatile uint8_t current_gear = 11;

// The MOb configured for RX of gear and clutch instructions.
volatile uint8_t gc_MOb;
// The MOb configured for RX of brake pressure.
volatile uint8_t brk_MOb;
// The MOb configured for RX of current gear.
volatile uint8_t dta_MOb;

int main(void) {
	io_init(); // initialise LUR_io.
	adc_init(); // initialise LUR7_adc.
	can_init(); // initialise LUR7_CAN.
	timer0_init(); // initialise LUR7_timer0.
	timer1_init(ON); // initialise LUR7_timer1.

	power_off_default(); // power off unused periferals.

	gc_MOb = can_setup_rx(CAN_GEAR_ID, CAN_GEAR_CLUTCH_MASK, CAN_GEAR_CLUTCH_DLC); // Reception of gear and clutch instructions.
	brk_MOb = can_setup_rx(CAN_FRONT_LOG_STEER_BRAKE_ID, CAN_FRONT_LOG_BRAKE_MASK, CAN_FRONT_LOG_DLC); // Reception of brake light instructions.
	dta_MOb = can_setup_rx(CAN_DTA_GEAR_ID, CAN_DTA_GEAR_MASK, CAN_DTA_DLC); // Reception of current gear from DTA.

	interrupts_on(); // enable interrupts.
	can_enable(); // enable CAN.

	while (1) {
		if (gear_up_flag) { // if gear_up_flag is set.
			gear_up(current_gear); // change up a gear.
			gear_up_flag = FALSE;  // clear gear_up_flag.
		}
		if (gear_down_flag) { // if gear_down_flag is set.
			gear_down(current_gear); // change down a gear.
			gear_down_flag = FALSE; // clear gear_down_flag.
		}
		if (gear_neutral_flag) { // if gear_down_flag is set.
			gear_neutral(current_gear); // change down a gear.
			gear_neutral_flag = FALSE; // clear gear_down_flag.
		}
	}
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

void timer1_isr_100Hz(uint8_t interrupt_nbr) {}

//see gear_clutch.c
//void timer0_isr_stop(void) {}

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {
	if (mob == gc_MOb) { // gc_MOb receives a message
		if (id == CAN_GEAR_ID) { // gear change message
			uint16_t gear_data = ((uint16_t) data[1] << 8) | data[0];
			if (gear_data == CAN_MSG_GEAR_UP) { // if message is CAN_MSG_GEAR_UP, set \ref gear_up_flag to TRUE.
				gear_up_flag = TRUE;
			} else if (gear_data == CAN_MSG_GEAR_DOWN) { // if message is CAN_MSG_GEAR_DOWN, set \ref gear_down_flag to TRUE.
				gear_down_flag = TRUE;
			} else if (gear_data == CAN_MSG_GEAR_NEUTRAL) { // if message is CAN_MSG_GEAR_NEUTRAL, set \ref gear_down_flag to TRUE.
				gear_neutral_flag = TRUE;
			}
		} else if (id == CAN_CLUTCH_ID) { // if message ID is CAN_CLUTCH_ID
			uint16_t clutch_p = ((uint16_t) data[1] << 8) | data[0];
			clutch_set(clutch_p); // set clutch pwm.
		}
	}

	if (mob == brk_MOb) { // brk_MOb receives a message
		uint16_t brake_p = ((uint16_t) data[3] << 8) | data[2]; // reconstruct brake pressure
		brake_light(brake_p); // control brake light
	}

	if (mob == dta_MOb) { // dta_MOb receives a message
		current_gear = data[0];
	}
}

void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}
void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
