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
#include "gear_launch.h"
#include "clutch.h"

//! Flag to set if signal to change up is received.
volatile uint8_t gear_up_flag = FALSE;
//! Flag to set if signal to change down is received.
volatile uint8_t gear_down_flag = FALSE;
//! Flag to set if signal to change to neutral (single attempt) is received.
volatile uint8_t gear_neutral_single_flag = FALSE;
//! Flag to set if signal to change to neutral (repeated attempts) is received.
volatile uint8_t gear_neutral_repeat_flag = FALSE;

//! Whether the DTA is in failsafe mode.
volatile uint8_t failsafe_dta = FALSE;
//! Counter to put DTA is in failsafe mode.
volatile uint8_t failsafe_dta_counter = 0;

//! The MOb configured for RX of gear, clutch and launch control instructions.
volatile uint8_t gcl_MOb;
//! The MOb configured for RX of current gear.
volatile uint8_t dta_MOb;

int main(void) {
	io_init();
	adc_init();
	ancomp_init();
	can_init();
	timer0_init();
	timer1_init(ON);

	power_off_default();

	gcl_MOb = can_setup_rx(CAN_GEAR_ID, CAN_GEAR_CLUTCH_LAUNCH_MASK, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	dta_MOb = can_setup_rx(CAN_DTA_ID, CAN_DTA_MASK, CAN_DTA_DLC);

	clutch_init();
	interrupts_on();
	can_enable();

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
		if (gear_neutral_repeat_flag) {
			gear_neutral_repeat_linear();
			//gear_neutral_repeat_bisect();
			gear_neutral_repeat_flag = FALSE;
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

void timer1_isr_100Hz(uint8_t interrupt_nbr) {
	if (!failsafe_dta && ++failsafe_dta_counter == 100) {
		failsafe_dta = TRUE;
		can_free_rx(dta_MOb);
		set_current_gear(13);
		set_current_revs(13000);
	}
	
	uint32_t filter = (uint32_t) clutch_get_filtered_right();
	can_setup_tx(CAN_REAR_LOG_FILTER_ID, (uint8_t *) &filter, CAN_REAR_LOG_DLC);
	uint32_t dutycycle = (uint32_t) clutch_get_dutycycle_right();
	can_setup_tx(CAN_REAR_LOG_DUTYCYCLE_ID, (uint8_t *) &dutycycle, CAN_REAR_LOG_DLC);
}

//see gear_launch.c
//void timer0_isr_stop(void) {}

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {
	if (mob == gcl_MOb) {
		if (id == CAN_GEAR_ID) {
			if (can_data_equals(CAN_MSG_GEAR_UP, data, dlc)) {
				gear_up_flag = TRUE;
			} else if (can_data_equals(CAN_MSG_GEAR_DOWN, data, dlc)) {
				gear_down_flag = TRUE;
			} else if (can_data_equals(CAN_MSG_GEAR_NEUTRAL_SINGLE, data, dlc)) {
				gear_neutral_single_flag = TRUE;
			} else if (can_data_equals(CAN_MSG_GEAR_NEUTRAL_REPEAT, data, dlc)) {
				gear_neutral_repeat_flag = TRUE;
			}
		} else if (id == CAN_CLUTCH_ID) {
			//uint16_t clutch_left = ((uint16_t) data[1] << 8) | data[0];
			uint16_t clutch_right = ((uint16_t) data[1] << 8) | data[0];
			//clutch_filter_left(clutch_left);
			clutch_filter_right(clutch_right);
			//clutch_dutycycle_left();
			clutch_dutycycle_right();
			//clutch_set_dutycycle();
			timer1_dutycycle(clutch_get_dutycycle_right());
		}
	} else if (mob == dta_MOb) {
		failsafe_dta_counter = 0;
		if (id == 0x2000) {
			set_current_revs(((uint16_t) data[0] << 8) | data[1]);
		}
		if (id == 0x2003) {
			set_current_gear(data[7]);
		}
	}
}

void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}