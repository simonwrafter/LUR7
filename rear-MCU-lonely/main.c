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

volatile uint8_t failsafe_front = TRUE;
volatile uint8_t failsafe_mid = TRUE;
volatile uint8_t failsafe_dta = TRUE;


int main(void) {
	io_init(); // initialise LUR_io.
	adc_init(); // initialise LUR7_adc.
	timer0_init(); // initialise LUR7_timer0.
	timer1_init(ON); // initialise LUR7_timer1.

	power_off_default(); // power off unused periferals.

	pc_int_on(BAK_IN_GEAR_UP); // gear up backup
	pc_int_on(BAK_IN_GEAR_DOWN); // gear down backup
	pc_int_on(BAK_IN_NEUTRAL); // gear neutral backup
	
	interrupts_on(); // enable interrupts.
	can_enable(); // enable CAN.

	while (1) {
		if (gear_up_flag) { // if gear_up_flag is set.
			gear_up(); // change up a gear.
			gear_up_flag = FALSE;  // clear gear_up_flag.
		}
		if (gear_down_flag) { // if gear_down_flag is set.
			gear_down(); // change down a gear.
			gear_down_flag = FALSE; // clear gear_down_flag.
		}
		if (gear_neutral_flag) { // if gear_down_flag is set.
			gear_neutral(current_gear); // change down a gear.
			gear_neutral_flag = FALSE; // clear gear_down_flag.
		}

		if (failsafe_front) {
			uint16_t brake = adc_get(BAK_IN_BRAKE); // update the brake pressure value.
			brake_light(brake); // set brake light on/off.
		}
		
		if (failsafe_mid) {
			uint16_t clutch = adc_get(BAK_IN_CLUTCH); // update clutch position value.
			clutch_set(clutch); // set clutch pwm.
		}
		
		if (failsafe_dta) {
			current_gear = 11;
		}
	}
	return 0;
}

void pcISR_in1(void) {}
void pcISR_in2(void) {}

void pcISR_in3(void) {
	if (get_input(IN3)) {
		gear_up_flag = TRUE;
	}
}

void pcISR_in4(void) {}

void pcISR_in5(void) {
	if (get_input(IN5)) {
		gear_down_flag = TRUE;
	}
}

void pcISR_in6(void) {}

void pcISR_in7(void) {
	if (get_input(IN7)) {
		gear_neutral_flag = TRUE;
	}
}

void pcISR_in8(void) {}
void pcISR_in9(void) {}

void timer1_isr_100Hz(uint8_t interrupt_nbr) {}

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
