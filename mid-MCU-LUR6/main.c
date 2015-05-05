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

//! Atomically written copy of right clutch position sensor value.
volatile uint16_t clutch_pos_atomic = 0;

volatile uint8_t busy_gear = FALSE;
/*volatile uint8_t reset_flag = FALSE;
volatile uint16_t filter = 0;*/
void ugly_reset(void);

int main(void) {
	io_init();
	adc_init();
	ancomp_init();
	can_init();
	timer0_init();
	timer1_init(OFF);

	power_off_default();
	power_off_timer0();

	//can_setup_rx(CAN_REAR_LOG_FILTER_ID, 0xFFFFFFFF, CAN_REAR_LOG_DLC);
	
	ext_int_on(IO_GEAR_UP, 1, 1);
	ext_int_on(IO_GEAR_DOWN, 1, 1);
	//ext_int_on(IO_GEAR_NEUTRAL, 1, 1); //moved
	pc_int_on(IO_GEAR_NEUTRAL);
	pc_int_on(IN2);
	
	interrupts_on();
	can_enable();

	while (1) {
		uint16_t clutch_pos_right = adc_get(IO_CLUTCH_RIGHT);
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			clutch_pos_atomic = clutch_pos_right;
		} // end ATOMIC_BLOCK
		/*
		if (reset_flag) {
			ugly_reset();
			reset_flag = FALSE;
		}*/
	}
	return 0;
}

void ugly_reset(void) {
	set_output(OUT8, GND);
	ext_int_off(IO_GEAR_UP);
	ext_int_off(IO_GEAR_DOWN);
	_delay_ms(1);
	set_output(OUT8, TRI);
	ext_int_on(IO_GEAR_UP, 1, 1);
	ext_int_on(IO_GEAR_DOWN, 1, 1);
}

void timer1_isr_100Hz(uint8_t interrupt_nbr) {
	uint32_t c_data = ((uint32_t) 0 << 16) | (uint16_t) clutch_pos_atomic;
	can_setup_tx(CAN_CLUTCH_ID, (uint8_t *) &c_data, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	//can_setup_tx(0x10203, (uint8_t *) &filter, 2);
}

void timer0_isr_stop(void) {}

ISR (INT_GEAR_UP) { //IN9
	if (!busy_gear) {
		if (can_setup_tx(CAN_GEAR_ID, CAN_MSG_GEAR_UP, CAN_GEAR_CLUTCH_LAUNCH_DLC) == 0xFF) {
			return;
		} else {
			busy_gear = TRUE;
		}
	}
	
}

ISR (INT_GEAR_DOWN) { //IN8
	if (busy_gear) {
		return;
	} else {
		if (can_setup_tx(CAN_GEAR_ID, CAN_MSG_GEAR_DOWN, CAN_GEAR_CLUTCH_LAUNCH_DLC) == 0xFF) {
			return;
		} else {
			busy_gear = TRUE;
		}
	}
}

/*
ISR (INT_GEAR_NEUTRAL) { //IN5 moved to pc_int on IN6
	if (!get_input(IO_ALT_BTN)) {
		can_setup_tx(CAN_GEAR_ID, CAN_MSG_GEAR_NEUTRAL_SINGLE, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	} else {
		can_setup_tx(CAN_GEAR_ID, CAN_MSG_GEAR_NEUTRAL_REPEAT, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	}
}
*/

void pcISR_in1(void) {}
void pcISR_in2(void) {
	if (get_input(IN2)) {
		ugly_reset();
	}
}
void pcISR_in3(void) {}
void pcISR_in4(void) {}
void pcISR_in5(void) {}

void pcISR_in6(void) {
	if (get_input(IN6)) {
		if (!get_input(IO_ALT_BTN)) {
			can_setup_tx(CAN_GEAR_ID, CAN_MSG_GEAR_NEUTRAL_SINGLE, CAN_GEAR_CLUTCH_LAUNCH_DLC);
		} else {
			can_setup_tx(CAN_GEAR_ID, CAN_MSG_GEAR_NEUTRAL_REPEAT, CAN_GEAR_CLUTCH_LAUNCH_DLC);
		}
	}
}

void pcISR_in7(void) {}
void pcISR_in8(void) {}
void pcISR_in9(void) {}

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {
	/*
	if (id == CAN_REAR_LOG_FILTER_ID) {
		filter = ((uint16_t) data[1] << 8) | data[0];
		if (clutch_pos_atomic - filter > 50 || clutch_pos_atomic - filter < -50) {
			reset_flag = TRUE;
		}
	}*/
}

void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {
	if (id == CAN_GEAR_ID) {
		busy_gear = FALSE;
	}
}
void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
