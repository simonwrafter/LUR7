/*
 * main.c - The main file for the LUR7 logger project.
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
#include "spi_routines.h"
#include "sd_routines.h"
#include "fat32.h"

volatile uint16_t accx_atomic = 0;
volatile uint16_t accy_atomic = 0;
volatile uint16_t accz_atomic = 0;

volatile uint8_t mob_dta = 0;
volatile uint8_t mob_front = 0;
volatile uint8_t mob_mid = 0;
volatile uint8_t mob_rear = 0;
volatile uint8_t mob_log = 0;

volatile uint8_t mob_dta_str[60] =   "00id, 00000000, 0d1, 0d2, 0d3, 0d4, 0d5, 0d6, 0d7, 0d8, \n";
volatile uint8_t mob_front_str[50] = "00id, 00000000, 0d1, 0d2, 0d3, 0d4, \n";
volatile uint8_t mob_mid_str[50] =   "00id, 00000000, 0d1, 0d2, 0d3, 0d4, \n";
volatile uint8_t mob_rear_str[50] =  "00id, 00000000, 0d1, 0d2, 0d3, 0d4, \n";
volatile uint8_t acc_xyz_str[50] =   "0000, 00000000, 0d1, 0d2, 0d3, 0d4, \n";

volatile uint8_t mob_dta_flag = 0;
volatile uint8_t mob_front_flag = 0;
volatile uint8_t mob_mid_flag = 0;
volatile uint8_t mob_rear_flag = 0;
volatile uint8_t mob_log_flag = 0;
volatile uint8_t acc_xyz_flag = 0;

volatile uint32_t clock = 0;

int main(void) {
	uint8_t option, error, data, FAT32_active;
	uint16_t i;
	uint8_t fileName[13];
	
	//**********************
	// Init main system functionality
	//**********************
	
	io_init();
	adc_init();
	ancomp_init();
	can_init();
	timer1_init(OFF);
	power_off_default();
	power_off_timer0();
	
	//**********************
	// Init SPI and SD card
	//**********************
	
	SPI_init();
	
	cardType = 0; //idioti
	
	for (i=0; i<10; i++) {
		error = SD_init();
		if (!error) {
			break;
		}
	}

	error = getBootSectorData(); //read boot sector and keep necessary data in global variables
	if(error) {
		//FAT32 incompatible drive
		while (1) {
			;
		}
	}
	
	SPI_high_speed();
	_delay_ms(1);   	//some delay for settling new spi speed
	
	//**********************
	// Finalise Init
	//**********************
	
	mob_dta = can_rx_setup(CAN_DTA_ID, 0xFFFFFFF0, CAN_DTA_DLC) // 0x2000
	mob_front = can_rx_setup(CAN_FRONT_LOG_SPEED_ID, 0xFFFFFFF0, CAN_FRONT_LOG_DLC) // 0x4000
	mob_mid = can_rx_setup(CAN_GEAR_ID, CAN_GEAR_CLUTCH_LAUNCH_MASK, CAN_GEAR_CLUTCH_LAUNCH_DLC) // 0x1500
	mob_rear = can_rx_setup(CAN_REAR_LOG_SPEED_ID, 0xFFFFFFF0, CAN_REAR_LOG_DLC) // 0x4500
	mob_log = can_rx_setup(CAN_LOG_ID, CAN_LOG_MASK, CAN_LOG_DLC); // start/stop instruction for logging
	
	interrupts_on();
	can_enable();
	
	while (1) {
		uint16_t accx = adc_get(ACC_X);
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			accx_atomic = accx;
		} // end ATOMIC_BLOCK
		uint16_t accy = adc_get(ACC_Y);
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			accy_atomic = accy;
		} // end ATOMIC_BLOCK
		uint16_t accz = adc_get(ACC_Z);
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			accz_atomic = accz;
		} // end ATOMIC_BLOCK
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

void timer1_isr_100Hz(uint8_t interrupt_nbr) {
	clock++;
	
}
void timer0_isr_stop(void) {}

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
