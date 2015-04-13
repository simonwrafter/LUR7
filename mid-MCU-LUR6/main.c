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

#include "../header_and_config/LUR7.h"
#include "config.h"

//! The MOb configured for RX of logging start/stop instructions.
volatile uint8_t CAN_DTA_MOb;
//! Right clutch position sensor value.
volatile uint16_t clutch_pos_right = 0;
//! Atomically written copy of right clutch position sensor value.
volatile uint16_t clutch_pos_right_atomic = 0;

int main(void) {
	io_init();
	adc_init();
	ancomp_init();
	can_init();
	timer1_init(OFF);

	power_off_default();
	power_off_timer0();

	CAN_DTA_MOb = can_setup_rx(CAN_DTA_ID, CAN_DTA_MASK, CAN_DTA_DLC);

	ext_int_on(IO_GEAR_UP, 1, 1);
	ext_int_on(IO_GEAR_DOWN, 1, 1);
	ext_int_on(IO_GEAR_NEUTRAL, 1, 1);

	interrupts_on();
	can_enable();

	while (1) {
		clutch_pos_right = adc_get(IO_CLUTCH_RIGHT);
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			clutch_pos_right_atomic = clutch_pos_right;
		} // end ATOMIC_BLOCK
	}
	return 0;
}

void timer1_isr_100Hz(uint8_t interrupt_nbr) {
	uint32_t c_data = ((uint32_t) 0 << 16) | clutch_pos_right_atomic;
	can_setup_tx(CAN_CLUTCH_ID, (uint8_t *) &c_data, CAN_GEAR_CLUTCH_LAUNCH_DLC);
}

void timer0_isr_stop(void) {}

ISR (INT_GEAR_UP) { //IN9
	can_setup_tx(CAN_GEAR_ID, (uint8_t *) &CAN_MSG_GEAR_UP, CAN_GEAR_CLUTCH_LAUNCH_DLC);
}

ISR (INT_GEAR_DOWN) { //IN8
	can_setup_tx(CAN_GEAR_ID, (uint8_t *) &CAN_MSG_GEAR_DOWN, CAN_GEAR_CLUTCH_LAUNCH_DLC);
}

ISR (INT_GEAR_NEUTRAL) { //IN5
	if (!get_input(IO_ALT_BTN)) {
		can_setup_tx(CAN_GEAR_ID, (uint8_t *) &CAN_MSG_GEAR_NEUTRAL_SINGLE, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	} else {
		can_setup_tx(CAN_GEAR_ID, (uint8_t *) &CAN_MSG_GEAR_NEUTRAL_REPEAT, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	}
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

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
