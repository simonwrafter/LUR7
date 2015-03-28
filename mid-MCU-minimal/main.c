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
#include "display.h"

// The MOb configured for RX of logging start/stop instructions.
volatile uint8_t CAN_DTA_MOb;
// Flag set when new information has been received and the panel is ready to be updated
volatile uint8_t new_info = TRUE;
// Clutch position sensor value.
volatile uint16_t clutch_pos = 0;
// Atomically written copy of clutch position sensor value.
volatile uint16_t clutch_pos_atomic = 0;


int main(void) {
	io_init(); // initialise LUR_io.
	adc_init(); // initialise LUR7_adc.
	can_init(); // initialise LUR7_CAN.
	timer1_init(ON); // initialise LUR7_timer0.

	power_off_default(); // power off unused periferals.
	power_off_timer0(); // no PWM output is required, so LUR7_timer1 is powered off.

	CAN_DTA_MOb = can_setup_rx(CAN_DTA_ID, CAN_DTA_MASK, CAN_DTA_DLC); // Reception of DTA packages, ID 0x2000-3.

	ext_int_on(IO_GEAR_UP, 1, 1); // Gear up, rising flank trigger external interrupt
	ext_int_on(IO_GEAR_DOWN, 1, 1); // Gear down, rising flank trigger external interrupt
	ext_int_on(IO_GEAR_NEUTRAL, 1, 1); // neutral Gear, rising flank trigger external interrupt

	interrupts_on(); // enable interrupts.
	can_enable(); // enable CAN.

	while (1) {
		clutch_pos = adc_get(IO_CLUTCH); // get clutch paddle position
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			clutch_pos_atomic = clutch_pos; // copy value to atomic variable
		} // end ATOMIC_BLOCK

		if (new_info) {
			new_info = FALSE; //clear flag
			update_display(); // update panel
		}
	}
	return 0;
}

void timer1_isr_100Hz(uint8_t interrupt_nbr) {
	timer1_dutycycle(clutch_pos_atomic * 10);
	can_setup_tx(CAN_CLUTCH_ID, (uint8_t *) &clutch_pos_atomic, CAN_GEAR_CLUTCH_DLC);
}

void timer0_isr_stop(void) {}

ISR (INT_GEAR_UP) { //IN9
	can_setup_tx(CAN_GEAR_ID, (uint8_t *) &CAN_MSG_GEAR_UP, CAN_GEAR_CLUTCH_DLC);
}
ISR (INT_GEAR_DOWN) { //IN8
	can_setup_tx(CAN_GEAR_ID, (uint8_t *) &CAN_MSG_GEAR_DOWN, CAN_GEAR_CLUTCH_DLC);
}
ISR (INT_GEAR_NEUTRAL) { //IN5
	can_setup_tx(CAN_GEAR_ID, (uint8_t *) &CAN_MSG_GEAR_NEUTRAL, CAN_GEAR_CLUTCH_DLC);
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

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {
	uint8_t id_lsb = ((uint8_t *) &id)[3]; // extract message specifier.
	if (mob == CAN_DTA_MOb) { // if received from DTA:
		switch (id_lsb) {
			case 0 : // ID = 0x2000.
				update_RPM((data[1] << 8) | data[0]); // extract RPM.
				update_watertemp((data[5] << 8) | data[4]); // extract water temperature [C].
				new_info = TRUE; // set flag to update panel
				break;
			case 1 : // ID = 0x2001.
				update_speed((data[5] << 8) | data[4]);  // extract speed [km/h * 10]
				break;
			case 2 : // ID = 0x2002.
				update_oiltemp((data[3] << 8) | data[2]);  // extract oil temperature [C].
				break;
			case 3 : // ID = 0x2003.
				update_gear(data[0]); // extract current gear.
				break;
			default :
				break;
		}
	}
}
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
