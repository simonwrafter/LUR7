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
#include "shiftregister.h"
#include "display.h"

static uint8_t CAN_DTA_MOb; // message object for the DTA's CAN messages
static uint8_t logging = FALSE; // message object for the DTA's CAN messages

int main(void) {
	//init +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	io_init(); //init outputs
	adc_init(); //init A/D conversion
	ancomp_init(); //init Analog Comparator
	can_init(); //init CAN
	timer0_init(); //init timer interrupts
	//timer1_init(); // if PWM output
	power_off_default(); //turn off unused stuff
	power_off_timer1(); //if no PWM output
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	//setup ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CAN receivers
	//    DTA listen for packages 0x2000-3 (mask = 0b11..11100)
	CAN_DTA_MOb = can_setup_rx(CAN_DTA_ID, CAN_DTA_MASK, CAN_DTA_DLC);
	
	// Input interrupts
	ext_int_on(IO_GEAR_UP, 1, 1); // Gear up, rising flank trigger
	ext_int_on(IO_GEAR_DOWN, 1, 1); // Gear down, rising flank trigger
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//enable, LAST!!! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	interrupts_on();
	can_enable();
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	//loop variables +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	//loop +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	while (1) {
		update_display();
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	return 0;
}

// Interrupts

void timer0_isr_100Hz(uint8_t interrupt_nbr) {
	//while adc_get is used here: DO THIS LAST!!!
	uint16_t clutch_pos = adc_get(IO_CLUTCH); //get clutch position
	can_setup_tx(CAN_CLUTCH_ID, (uint8_t *) &clutch_pos, CAN_GEAR_CLUTCH_DLC);
}

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
//! General purpos button, no job yet
void pcISR_in3(void) {}
//! Log start/stop button
void pcISR_in4(void) {
	if (logging) {
		can_setup_tx(CAN_LOG_ID, (uint8_t *) &CAN_MSG_LOG_STOP, CAN_LOG_DLC);
	} else {
		can_setup_tx(CAN_LOG_ID, (uint8_t *) &CAN_MSG_LOG_START, CAN_LOG_DLC);
	}
}
//! \warning in5 used as external interrupt
void pcISR_in5(void) {}
//! \warning in6 used as analog input
void pcISR_in6(void) {}
void pcISR_in7(void) {}
//! \warning in8 used as external interrupt
void pcISR_in8(void) {}
//! \warning in9 used as external interrupt
void pcISR_in9(void) {}

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {
	uint8_t id_lsb = ((uint8_t *) &id)[3];
	if (mob == CAN_DTA_MOb) {
		switch (id_lsb) {
			case 0 :
				update_RPM((data[1] << 8) | data[0]); // rpm
				// TPS = (data[3] << 8) | data[2]; // %
				update_watertemp((data[5] << 8) | data[4]); // C
				// air temp = (data[7] << 8) | data[6] // C
				break;
			case 1 :
				// MAP = (data[1] << 8) | data[0] // KPa
				// Lambda x 1000 = (data[3] << 8) | data[2] // x 1000
				update_speed((data[5] << 8) | data[4]); // Kph x 10
				// Oil P = (data[7] << 8) | data[6] // KPa
				break;
			case 2 :
				// Fuel P = (data[1] << 8) | data[0] // KPa
				update_oiltemp((data[3] << 8) | data[2]); // C
				// Volts x 10 = (data[5] << 8) | data[4] // V x 10
				// Fuel Con. L/Hr x 10 = (data[7] << 8) | data[6] // L/Hr x 10
				break;
			case 3 :
				update_gear(data[1]); // gear
				// Advance = (data[3] << 8) | data[2] // deg x 10
				// Injection = (data[5] << 8) | data[4] // ms x 10
				// Fuel Con. = (data[7] << 8) | data[6] // L/100Km x 10
				break;
				/*
			case 4 :
				// Ana1 = (data[1] << 8) | data[0] // mV
				// Ana2 = (data[3] << 8) | data[2] // mV
				// Ana3 = (data[5] << 8) | data[4] // mV
				// Cam Advance = (data[7] << 8) | data[6] // x 10
				break;
			case 5 :
				// Cam Targ x 10 = (data[1] << 8) | data[0]
				// Cam PWM x 10 = (data[3] << 8) | data[2]
				// Crank Errors = (data[5] << 8) | data[4]
				// Cam Errors = (data[7] << 8) | data[6]
				break;
			*/
			default :
				break;
		}
	}
}

void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
