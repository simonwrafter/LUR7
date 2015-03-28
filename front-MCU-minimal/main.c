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

// Brake pressure variable.
volatile uint16_t brake = 0;
// An atomically written and safe copy of brake pressure sensor for use in interrupt.
volatile uint16_t brake_atomic = 0;

int main(void) {
	io_init(); // initialise LUR_io.
	adc_init(); // initialise LUR7_adc.
	can_init(); // initialise LUR7_CAN.
	timer1_init(OFF); // initialise LUR7_timer1.
	
	power_off_default(); // power off unused periferals.
	power_off_timer0(); // LUR7_timer0 is powered off.

	interrupts_on(); // enable interrupts.
	can_enable(); // enable CAN.

	while (1) {
		brake = adc_get(BRAKE_PRESSURE); //! <li> update the brake pressure value.
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			brake_atomic = brake; //! <li> atomic copy of brake pressure value.
		} // end ATOMIC_BLOCK
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
	if (((interrupt_nbr + 4) % 5) == 0) {
		uint32_t holder = ((uint32_t) brake_atomic << 16) | 0x0000; // build data
		can_setup_tx(CAN_FRONT_LOG_STEER_BRAKE_ID, (uint8_t *) &holder, CAN_FRONT_LOG_DLC); // send
	}
}

void timer0_isr_stop(void) {}
void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}
void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
